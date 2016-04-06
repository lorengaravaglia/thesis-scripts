#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctime>
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
	int				got_picture, videoindex;
};

cv::Mat m;

char filepath[] = "test.sdp";
const char hello[] = "1";

HANDLE hPipe;
DWORD dwWritten = 0;
clock_t begin, end;

int ret;
int randBase = 30;
int randValue = 0;


int startFFMPEG(FFMPEGData &data)
{
	data.pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&data.pFormatCtx, filepath, NULL, NULL) < 0)
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
	av_dump_format(data.pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	data.convert_ctx = sws_getContext(data.pCodecCtx->width, data.pCodecCtx->height, data.pCodecCtx->pix_fmt,
		data.pCodecCtx->width, data.pCodecCtx->height, data.dst_pixfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	return 0;
}


int main(int argc, char **argv)
{
	FFMPEGData vidData;

	av_register_all();
	avformat_network_init();

	hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	while (true)
	{
		randValue = rand() % 100 + randBase;

		if (startFFMPEG(vidData) < 0)
		{
			printf("should not get here.\n");
			return 0;
		}

		printf("time for this loop = %d", randValue);

		begin = clock();

		while (true)
		{
			if (av_read_frame(vidData.pFormatCtx, vidData.packet) != AVERROR(EAGAIN))
			{
				if (vidData.packet->stream_index == vidData.videoindex)
				{
					ret = avcodec_decode_video2(vidData.pCodecCtx, vidData.pFrame, &vidData.got_picture, vidData.packet);
					if (ret < 0)
					{
						printf("Decode Error.\n");
						return -1;
					}

					if (vidData.got_picture)
					{
						if (vidData.convert_ctx == NULL)
						{
							fprintf(stderr, "Cannot initialize the conversion context!\n");
							exit(1);
						}

						sws_scale(vidData.convert_ctx, vidData.pFrame->data, vidData.pFrame->linesize, 0,
							vidData.pFrame->height, vidData.dst.data, vidData.dst.linesize);

						m = cv::Mat(vidData.pFrame->height, vidData.pFrame->width, CV_8UC1,
							vidData.dst.data[0], vidData.dst.linesize[0]);

						cv::imshow("MyVid", m);
						cv::waitKey(20);
						end = clock();
						double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
						printf("elapsed time = %0.2f\n", elapsed_secs);

						if (elapsed_secs >= randValue)
						{
							if (hPipe != INVALID_HANDLE_VALUE)
							{
								WriteFile(hPipe,
									hello,				//"Hello Pipe\n",
									sizeof(hello),		// = length of string + terminating '\0' !!!
									&dwWritten,
									NULL);
								printf("wrote to pipe\n");
								//CloseHandle(hPipe);
							}
							break;
						}
					}
				}
				av_free_packet(vidData.packet);
			}
		}
		av_free_packet(vidData.packet);
		sws_freeContext(vidData.convert_ctx);
		av_frame_free(&vidData.pFrame);
		avcodec_close(vidData.pCodecCtx);
		avformat_close_input(&vidData.pFormatCtx);
	}
	return 0;
}