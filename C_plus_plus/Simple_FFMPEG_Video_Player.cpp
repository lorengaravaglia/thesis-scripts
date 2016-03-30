#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
//#include "SDL2/SDL.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")
//#pragma comment(lib, "SDL2.lib")
//#pragma comment(lib, "SDL2main.lib") 

	AVFormatContext	*pFormatCtx;
	int				 videoindex;
	unsigned int     i;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame			*pFrame;
	uint8_t			*out_buffer;
	AVPacket		*packet;
	AVFrame			dst;
	int				ret, got_picture;
	cv::Mat			m;

	struct SwsContext *convert_ctx;
	
	enum PixelFormat dst_pixfmt = PIX_FMT_GRAY8; //full color image.
	char filepath[]= "test.sdp"; 
	

int main(int argc, char **argv)
{
                                         
	av_register_all();
	avformat_network_init();

	pFormatCtx = avformat_alloc_context();
	
	if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL) < 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	if(avformat_find_stream_info(pFormatCtx,NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			videoindex=i;
			break;
		}
	}

	if(videoindex==-1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx=pFormatCtx->streams[videoindex]->codec;
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);

	if(pCodec==NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}

	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}
	
	pFrame=av_frame_alloc();

	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(dst_pixfmt, pCodecCtx->width, pCodecCtx->height));

	avpicture_fill((AVPicture *)&dst, out_buffer, dst_pixfmt, pCodecCtx->width, pCodecCtx->height);

	packet=(AVPacket *)av_malloc(sizeof(AVPacket));

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");

	convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
		pCodecCtx->width, pCodecCtx->height, dst_pixfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL); 

	while(true)
	{
		if (av_read_frame(pFormatCtx, packet) != AVERROR(EAGAIN))
		{
			if(packet->stream_index==videoindex)
			{
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
				if(ret < 0)
				{
					printf("Decode Error.\n");
					return -1;
				}

				if(got_picture)
				{
					if(convert_ctx == NULL)
					{
						fprintf(stderr, "Cannot initialize the conversion context!\n");
						exit(1);
					}

					sws_scale(convert_ctx, pFrame->data, pFrame->linesize, 0, pFrame->height,
							 dst.data, dst.linesize);

					m = cv::Mat(pFrame->height, pFrame->width, CV_8UC1, dst.data[0], dst.linesize[0]);
				
					imshow("MyVideo", m);
					cv::waitKey(20);
					//break;
				}
			}
			av_free_packet(packet);
		}
	}
	/*
	sws_freeContext(convert_ctx);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	*/
	return 0;
}