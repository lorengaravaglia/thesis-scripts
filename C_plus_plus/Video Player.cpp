#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctime>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>
#include <boost/chrono.hpp>

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

int packetNum = 5000;

struct FFMPEGData {
	AVFormatContext	*pFormatCtx;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame			*pFrame, dst;
	AVPacket		*packet;
	AVPacket* pack = new AVPacket[packetNum];

	struct SwsContext *convert_ctx;

	enum PixelFormat dst_pixfmt = PIX_FMT_GRAY8; //full color image.

	uint8_t			*out_buffer;
	int				got_picture, videoindex, restart;
	int             randValue, randBitrateValue, previousBitrate;
	char            filepath[15];
	clock_t         begin, end;
	cv::Mat         m;
	int				windowName;

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

	int startFFMPEG();
};

char hello[10];

HANDLE hPipe;
DWORD dwWritten = 0;

int ret;
int randBase = 30;
int randBitrateBase = 20;

int nodeNumber = 2;

FFMPEGData* vidData = new FFMPEGData[nodeNumber];

boost::thread_group plays;

int FFMPEGData::startFFMPEG()
{
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) < 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	videoindex = -1;
	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();

	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(dst_pixfmt, pCodecCtx->width, pCodecCtx->height));

	avpicture_fill((AVPicture *)&dst, out_buffer, dst_pixfmt, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, dst_pixfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	return 0;
}



void play(FFMPEGData &vidData)
{
	int i = 0;
	int count = 0;
	Sleep(1000);
	while (true)
	{
		printf("count = %d\n", count);
		if (vidData.pack[count].stream_index == vidData.videoindex)
		{
			ret = avcodec_decode_video2(vidData.pCodecCtx, vidData.pFrame, &vidData.got_picture, &vidData.pack[count]);
			if (ret < 0)
			{
				printf("Decode Error.\n");
				//return -1;
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

				//Convert the received frame to OpenCV format.
				vidData.m = cv::Mat(vidData.pFrame->height, vidData.pFrame->width, CV_8UC1,
					vidData.dst.data[0], vidData.dst.linesize[0]);
				char window[5];
				sprintf_s(window, "%d", vidData.windowName);
				cv::imshow(window, vidData.m);
				cv::waitKey(20);

			}
		}
		//av_free_packet(&pack[count]);
		if (count < packetNum)
		{
			count++;
		}
		else
		{
			count = 0;
		}
		Sleep(333);
	}

}


void thread(FFMPEGData &vidData)
{
	int counter = 0;
	// changed this to use member function.
	if (vidData.startFFMPEG() < 0)
	{
		printf("should not get here.\n");
	}
	plays.add_thread(new boost::thread(play, vidData));
	while (true)
	{
		printf("\t\tthread count = %d\n", counter);
		// Read frames from the stream.
		if (av_read_frame(vidData.pFormatCtx, &vidData.pack[counter]) != AVERROR(EAGAIN))
		{
			if (counter < packetNum)
			{
				counter++;
			}
			else
			{
				counter = 0;
			}
		}

		//Sleep(25);
	}
}



using namespace std;

int main(int argc, char **argv)
{
	int i = 0;
	boost::thread_group reads;
	
	//std::vector<FFMPEGData> vidData;
	//FFMPEGData* vidData = new FFMPEGData[nodeNumber];

	av_register_all();
	avformat_network_init();
	// Create a struct entry for every node.
	//vidData.push_back(FFMPEGData());


	for (i = 0; i < nodeNumber; i++)
	{
		// Set the sdp file for each node.
		sprintf_s(vidData[i].filepath, "test%d.sdp", (i + 1));

		// Print the file path for debugging.
		printf("Filepath = %s\n", vidData[i].filepath);

		vidData[i].restart = 0;
		vidData[i].windowName = i;

		reads.add_thread(new boost::thread(thread, boost::ref(vidData[i])));
		// plays.add_thread(new boost::thread(play, vidData[i]));
		//boost::scoped_thread<> t{ boost::thread{ thread } };
		//boost::scoped_thread<> f{ boost::thread{ play } };

	}
	reads.join_all();
	plays.join_all();
	//If the code ever reaches here close the pipe before finishing.
	delete[] vidData;
	return 0;
}