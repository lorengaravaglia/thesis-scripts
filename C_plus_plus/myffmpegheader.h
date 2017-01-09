#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
}

#include <vector>

//Loren OpenCV includes
#include "opencv2\core\core.hpp"
#include "opencv2\contrib\contrib.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")


/*
extern AVCodecContext		 *pCodecCtx;
extern char 				 filepath[];
extern int				     videoindex;
*/


//int packetNum = 5000;

//Loren: Added for running ffmpeg process.
struct FFMPEGData {
AVFormatContext		 *pFormatCtx;
AVCodecContext		 *pCodecCtx;
AVCodecContext		 *pCodecCtx1;
AVCodec				 *pCodec;
AVCodec				 *pCodec1;
AVPacket 			 pkt;
AVFrame				 *frame, *pFrame, *pFrameRGB, *pFrameH264;
AVCodec *codec;
AVCodecID codec_id;
AVCodecContext *c;
cv::Mat test;
cv::CascadeClassifier haar_cascade;

uint8_t			     *out_buffer;

struct SwsContext    *sws_ctx;
int64_t pts, dts, last_pts, last_dts;
double prevAppRate;
int noFaceCount, got_picture, frameNumber;
double nodeCorrect, nodeTotal, nodeAccuracy;

int					 videoindex, restart,frameCount, startH264;
//enum PixelFormat	 dst_pixfmt = PIX_FMT_BGR24;
char 				 filepath[100]; // = "C:\\OPNET\\14.5.A\\models\\std\\before GT30\\traf_gen\\test.sdp";


AVPacket* pack;// = new AVPacket[5000];

	FFMPEGData()
	{
		// Initialize some of the values to defaults.
		frameCount = 0;
		restart = 1;
		startH264 = 1;
		videoindex = 0;
		pts = 0;
		dts = 0;
		last_pts = 0;
		last_dts = 0;
		prevAppRate = 0;
		nodeCorrect = 0;
		nodeTotal = 0;
		nodeAccuracy = 0;
		noFaceCount = 0;
		sprintf_s(filepath, "None");
		codec_id = AV_CODEC_ID_H264;
		c = NULL;
		frame = NULL;
		pFrame = NULL;
		pFrameRGB = NULL;
		pFrameH264 = NULL;
		sws_ctx = NULL;
		//test = cv::Mat(480,640,CV_8U, double(0));
		got_picture = 0;
		frameNumber = 0;
	}

};


//extern std::vector<FFMPEGData> vidData;
extern FFMPEGData* vidData;

extern int f;
extern int allocateFlag;
