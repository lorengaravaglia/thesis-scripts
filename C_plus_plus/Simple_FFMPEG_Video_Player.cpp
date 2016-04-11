#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctime>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "SDL2/SDL.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib") 

struct FFMPEGData {
	AVFormatContext	*pFormatCtx;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame			*pFrame, dst;
	AVPacket		*packet;

	struct SwsContext *convert_ctx;

	enum PixelFormat dst_pixfmt = PIX_FMT_GRAY8; //full color image.

	uint8_t			*out_buffer;
	int				got_picture, videoindex, restart;
	int             randValue, randBitrateValue, previousBitrate;
	char            filepath[15];
	clock_t         begin, end;
	cv::Mat         m;

	FFMPEGData()
	{
		// Initialize some of the values to defaults.
		restart = 1;
		got_picture = 0;
		videoindex = 0;
		randBitrateValue = 0;
		previousBitrate = 0;
		randValue = 0;
		sprintf_s(filepath, "None");
	}
};

//cv::Mat m;

//char filepath[] = "test.sdp";
char hello[10];

HANDLE hPipe;
DWORD dwWritten = 0;

int ret;
int randBase = 30;
//int randValue = 0;
int randBitrateBase = 20;
//int randBitrateValue = 0;
//int previousBitrate = 0;
int nodeNumber = 2;

int startFFMPEG(FFMPEGData &data)
{
	data.pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&data.pFormatCtx, data.filepath, NULL, NULL) < 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if (avformat_find_stream_info(data.pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	data.videoindex = -1;
	for (unsigned int i = 0; i < data.pFormatCtx->nb_streams; i++)
	{
		if (data.pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			data.videoindex = i;
			break;
		}
	}

	if (data.videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	data.pCodecCtx = data.pFormatCtx->streams[data.videoindex]->codec;
	data.pCodec = avcodec_find_decoder(data.pCodecCtx->codec_id);

	if (data.pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if (avcodec_open2(data.pCodecCtx, data.pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	data.pFrame = av_frame_alloc();

	data.out_buffer = (uint8_t *)av_malloc(avpicture_get_size(data.dst_pixfmt, data.pCodecCtx->width, data.pCodecCtx->height));

	avpicture_fill((AVPicture *)&data.dst, data.out_buffer, data.dst_pixfmt, data.pCodecCtx->width, data.pCodecCtx->height);

	data.packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(data.pFormatCtx, 0, data.filepath, 0);
	printf("-------------------------------------------------\n");

	data.convert_ctx = sws_getContext(data.pCodecCtx->width, data.pCodecCtx->height, data.pCodecCtx->pix_fmt,
		data.pCodecCtx->width, data.pCodecCtx->height, data.dst_pixfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	return 0;
}

using namespace std;

int main(int argc, char **argv)
{
	std::vector<FFMPEGData> vidData;
	//FFMPEGData vidData;

	av_register_all();
	avformat_network_init();

	for (int i = 0; i < nodeNumber; i++)
	{
		// Create a struct entry for every node.
		vidData.push_back(FFMPEGData());

		// Set the sdp file for each node.
		sprintf_s(vidData[i].filepath, "test%d.sdp", (i+1));

		// Print the file path for debugging.
		printf("Filepath = %s\n", vidData[i].filepath);
	}

	hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	while (true)
	{
		// Loop through every node.
		for (int i = 0; i < nodeNumber; i++)
		{
			//get random time value for keeping the stream alive.
			vidData[i].randValue = randBase + (rand() % 100);

			printf("time for this loop = %d\n", vidData[i].randValue);

			//(re)start my ffmepg stream.
			if (vidData[i].restart == 1)
			{
				if (startFFMPEG(vidData[i]) < 0)
				{
					printf("should not get here.\n");
					return 0;
				}
				vidData[i].restart = 0;
			}
		}

		for (int i = 0; i < nodeNumber; i++)
		{
			// Getting the start time for the node.
			vidData[i].begin = clock();
		}

		while (true)
		{
			for (int i = 0; i < nodeNumber; i++)
			{
				// Read frames from the stream.
				if (av_read_frame(vidData[i].pFormatCtx, vidData[i].packet) != AVERROR(EAGAIN))
				{
					if (vidData[i].packet->stream_index == vidData[i].videoindex)
					{
						ret = avcodec_decode_video2(vidData[i].pCodecCtx, vidData[i].pFrame, &vidData[i].got_picture, vidData[i].packet);
						if (ret < 0)
						{
							printf("Decode Error.\n");
							return -1;
						}

						if (vidData[i].got_picture)
						{
							if (vidData[i].convert_ctx == NULL)
							{
								fprintf(stderr, "Cannot initialize the conversion context!\n");
								exit(1);
							}

							sws_scale(vidData[i].convert_ctx, vidData[i].pFrame->data, vidData[i].pFrame->linesize, 0,
								vidData[i].pFrame->height, vidData[i].dst.data, vidData[i].dst.linesize);

							//Convert the received frame to OpenCV format.
							vidData[i].m = cv::Mat(vidData[i].pFrame->height, vidData[i].pFrame->width, CV_8UC1,
								vidData[i].dst.data[0], vidData[i].dst.linesize[0]);
							char window[5];
							sprintf_s(window, "%d", i);
							cv::imshow(window, vidData[i].m);
							cv::waitKey(20);

							//Calculate the elapsed time.
							vidData[i].end = clock();
							double elapsed_secs = double(vidData[i].end - vidData[i].begin) / CLOCKS_PER_SEC;
							printf("elapsed time = %0.2f\n", elapsed_secs);

							// Have we reached the time to restart the stream?
							if (elapsed_secs >= vidData[i].randValue)
							{
								//Store the old bitrate so that we can do some hysteresis
								vidData[i].previousBitrate = vidData[i].randBitrateValue;

								//get random bitrate, for the next loop, with a specified minimum and intervals of 10.
								vidData[i].randBitrateValue = randBitrateBase + (10 * (rand() % 100));

								printf("random bitrate value = %d\n", vidData[i].randBitrateValue);

								if (vidData[i].randBitrateValue >= (vidData[i].previousBitrate + 50) || vidData[i].randBitrateValue <= (vidData[i].previousBitrate - 50))
								{
									//create the string with the correct format to be handled by the server program.
									sprintf_s(hello, "%d,%d", (i + 1), vidData[i].randBitrateValue);

									printf("%s\n", hello);

									//Check if the named pipe is still valid and then send the data.
									if (hPipe != INVALID_HANDLE_VALUE)
									{
										WriteFile(hPipe,
											hello,
											sizeof(hello),		// = length of string + terminating '\0' !!!
											&dwWritten,
											NULL);

										printf("wrote to pipe\n");
									}

									// Set the restart flag.
									vidData[i].restart = 1;
									//Now exit the loop to restart the stream.
									//break;
									goto end;
								}
							}
						}
					}
					av_free_packet(vidData[i].packet);
				}
			}
		}
		end:
		for (int i = 0; i < nodeNumber; i++)
		{
			if (vidData[i].restart == 1)
			{
				av_free_packet(vidData[i].packet);
				sws_freeContext(vidData[i].convert_ctx);
				av_frame_free(&vidData[i].pFrame);
				avcodec_close(vidData[i].pCodecCtx);
				avformat_close_input(&vidData[i].pFormatCtx);
			}
		}
	}
	//If the code ever reaches here close the pipe before finishing.
	CloseHandle(hPipe);
	return 0;
}