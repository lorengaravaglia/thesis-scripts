#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctime>
#include <vector>

#include "opencv2\core\core.hpp"
#include "opencv2\contrib\contrib.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

// Initalizing these to NULL prevents segfaults!
AVFormatContext   *pFormatCtx = NULL;
int               i, videoStream;
AVCodecContext    *pCodecCtxOrig = NULL;
AVCodecContext    *pCodecCtx = NULL;
AVCodec           *pCodec = NULL;
AVFrame           *pFrame = NULL;
AVFrame           *pFrameRGB = NULL;
AVPacket          packet;
int               frameFinished;
int               numBytes;
uint8_t           *buffer = NULL;
struct SwsContext *sws_ctx = NULL;

int64_t timeBase;
//cv::Mat test;
char printPath[100];


static int start(const char *filename)
{
	// Open video file
	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0)
		return -1; // Couldn't open file

				   // Retrieve stream information
	if (avformat_find_stream_info(pFormatCtx, NULL)<0)
		return -1; // Couldn't find stream information

				   // Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, filename, 0);

	// Find the first video stream
	videoStream = -1;
	for (unsigned int i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	if (videoStream == -1)
		return -1; // Didn't find a video stream

				   // Get a pointer to the codec context for the video stream
	pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
	// Find the decoder for the video stream
	pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
	if (pCodec == NULL) {
		fprintf(stderr, "Unsupported codec!\n");
		return -1; // Codec not found
	}
	// Copy context
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
		fprintf(stderr, "Couldn't copy codec context");
		return -1; // Error copying codec context
	}

	// Open codec
	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
		return -1; // Could not open codec

	// Allocate video frame
	pFrame = av_frame_alloc();

	return 1;
}


/*
* Video encoding example
*/
static void video_encode_example(const char *filename, int j, AVCodecID codec_id)
{
	AVCodec *codec;
	AVCodecContext *c = NULL;
	int i, ret, got_output, bitrate;
	FILE *f;
	AVFrame *frame;
	AVPacket pkt;
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	errno_t err;

	printf("Encode video file %s\n", filename);

	/* find the mpeg1 video encoder */
	codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}


		c = avcodec_alloc_context3(codec);
		if (!c) {
			fprintf(stderr, "Could not allocate video codec context\n");
			exit(1);
		}
		if (j == 0)
			bitrate = 80000;
		else
			bitrate = 200000;
		/* put sample parameters */
		c->bit_rate = bitrate;
		/* resolution must be a multiple of two */
		c->width = 640;
		c->height = 480;
		/* frames per second */
		c->time_base = { 1, 25 };
		/* emit one intra frame every ten frames
		* check frame pict_type before passing frame
		* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
		* then gop_size is ignored and the output of encoder
		* will always be I frame irrespective to gop_size
		*/
		c->gop_size = 10;
		c->max_b_frames = 1;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		//c->bit_rate_tolerance = 20000;
		//c->pix_fmt = AV_PIX_FMT_NV12;


		if (codec_id == AV_CODEC_ID_H264)
			av_opt_set(c->priv_data, "preset", "slow", 0);

		/* open it */
		if (avcodec_open2(c, codec, NULL) < 0) {
			fprintf(stderr, "Could not open codec\n");
			exit(1);
		}

		err = fopen_s(&f, filename, "a+b");
		if (!f) {
			fprintf(stderr, "Could not open %s\n", filename);
			exit(1);
		}

		frame = av_frame_alloc();
		if (!frame) {
			fprintf(stderr, "Could not allocate video frame\n");
			exit(1);
		}
		frame->format = c->pix_fmt;
		frame->width = c->width;
		frame->height = c->height;

		/* the image can be allocated by any means and av_image_alloc() is
		* just the most convenient way if av_malloc() is to be used */
		ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height,
			c->pix_fmt, 32);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate raw picture buffer\n");
			exit(1);
		}

		// Allocate an AVFrame structure
		pFrameRGB = av_frame_alloc();
		if (pFrameRGB == NULL)
			return;

		// Determine required buffer size and allocate buffer
		numBytes = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
			pCodecCtx->height);
		buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

		// Assign appropriate parts of buffer to image planes in pFrameRGB
		// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
		// of AVPicture
		avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_YUV420P,
			pCodecCtx->width, pCodecCtx->height);

		// initialize SWS context for software scaling
		sws_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height,
			pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height,
			AV_PIX_FMT_YUV420P,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
		);
		AVRational codec_time_base = pCodecCtx->time_base;
		/* encode 1 second of video */
		int changeRateFlag = 0;
		i = 0;
		long int time = pFormatCtx->duration;
		double totalTime = (double)pFormatCtx->duration / AV_TIME_BASE;
		printf("duration: %lld @ %d/sec (%.2f sec)\n", pFormatCtx->duration, AV_TIME_BASE, (double)pFormatCtx->duration / AV_TIME_BASE);
		printf("duration: %lld @ %d/sec (codec time base)\n", pFormatCtx->duration / AV_TIME_BASE * codec_time_base.den, codec_time_base.den);
		double fps = (double)codec_time_base.den / (double)codec_time_base.num;
		printf("time = %.2f\n", fps);
		double totalFrames = totalTime * fps;
		printf("total Frames = %.2f", totalTime * fps);
		int64_t pts, dts;
		int64_t last_pts = 0;
		int64_t last_dts = 0;

		timeBase = (int64_t(pFormatCtx->streams[videoStream]->time_base.num) * AV_TIME_BASE) / int64_t(pFormatCtx->streams[videoStream]->time_base.den);
		while (i < 16000)
		{//i <= totalFrames){

			printf("i = %d\n", i);
			av_init_packet(&pkt);
			pkt.data = NULL;    // packet data will be allocated by the encoder
			pkt.size = 0;

			fflush(stdout);
			double tim = 0;

			int frameIndex = 300;
			int64_t seekTarget = int64_t(frameIndex) * timeBase;

			if (av_read_frame(pFormatCtx, &packet) < 0) {
				//if (avformat_seek_file(pFormatCtx, videoStream, INT64_MIN, 60, INT64_MAX, 0) < 0)
				if (av_seek_frame(pFormatCtx, videoStream, frameIndex, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD) < 0)
				{
					printf("error\n");
					break;
				}
				else 
				{
					printf("succeeded\n");
					printf("dts = %d, last_dts = %d, pts = %d, last_pts = %d\n", (int)dts, (int)last_dts, (int)pts, (int)last_pts);
					last_dts += dts;
					last_pts += pts;
					printf("dts = %d, last_dts = %d, pts = %d, last_pts = %d\n", (int)dts, (int)last_dts, (int)pts, (int)last_pts);

					avcodec_flush_buffers(pCodecCtx);

					av_read_frame(pFormatCtx, &packet);
				}

				//if (retur <= 0)
				//	break;
			}
			//packet.flags |= AV_PKT_FLAG_KEY;
			pts = packet.pts;
			printf("pts = %lld\n", pts);
			//packet.pts += last_pts;
			dts = packet.dts;
			//packet.dts += last_dts;
			// Is this a packet from the video stream?
			if (packet.stream_index == videoStream) {
				// Decode video frame
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				int64_t received_timestamp = pFrame->pkt_pts;

				// Did we get a video frame?
				if (frameFinished) {
					// Convert the image from its native format to RGB
					sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
						pFrame->linesize, 0, pCodecCtx->height,
						frame->data, frame->linesize);
					//av_free(pFrame->data[0]);
				}
				
			}

			av_free_packet(&packet);
			//test = cv::Mat(pCodecCtx->height, pCodecCtx->width, CV_8U, frame->data[0], frame->linesize[0]);
			//sprintf(printPath, "G:\\Masters_Thesis_Files\\Honda_Database\\TestImg\\node0\\testEncode%d.jpg", i);

			//cv::imwrite(printPath, test);

			frame->pts = i; //pFrame->pkt_pts;

			/* encode the image */
			ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}

			if (got_output) {
				printf("j = %d Write frame %3d (size=%5d)\n", j, i, pkt.size);
				fwrite(pkt.data, 1, pkt.size, f);
				av_free_packet(&pkt);
				//av_packet_unref(&pkt);
			}
			i++;
		}
	
		/* get the delayed frames
		for (got_output = 1; got_output; i++) {
			fflush(stdout);

			ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				exit(1);
			}

			if (got_output) {
				printf("j = %d Write frame %3d (size=%5d)\n", j, i, pkt.size);
				fwrite(pkt.data, 1, pkt.size, f);
				av_packet_unref(&pkt);
			}
		}*/

		/* add sequence end code to have a real mpeg file */
		fwrite(endcode, 1, sizeof(endcode), f);
		fclose(f);

		avcodec_close(c);
		av_free(c);
		av_freep(&frame->data[0]);
		av_frame_free(&frame);
		printf("\n");
}

static void stop()
{
	// Free the RGB image
	av_free(buffer);
	av_frame_free(&pFrameRGB);

	// Free the YUV frame
	av_frame_free(&pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);
	avcodec_close(pCodecCtxOrig);

	// Close the video file
	avformat_close_input(&pFormatCtx);
}

int main()
{
	int ret;
	/* register all the codecs */
	av_register_all();
	//avcodec_register_all();

	char* filename = "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi";
	char* output;

	for (int j = 0; j < 1; j++)
	{
		ret = start(filename);

		if (ret == -1) {
			/* end early */
			return 0;
		}

		if (j == 0)
		{
			//output = "test.h264";
			output = "test2.mpg";
		}
		else
		{
			//output = "test1.h264";
			output = "test1.mpg";
		}
		//video_encode_example(output, j, AV_CODEC_ID_H264);
		video_encode_example(output, j, AV_CODEC_ID_MPEG1VIDEO);

		stop();

	}

	return 0;
}
