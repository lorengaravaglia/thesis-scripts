#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
}

#include <vector>

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")


/*
extern AVCodecContext		 *pCodecCtx;
extern char 				 filepath[];
extern int				     videoindex;
*/

//Loren: Added for running ffmpeg process.
struct FFMPEGData {
AVFormatContext		 *pFormatCtx;
AVCodecContext		 *pCodecCtx;
AVCodec				 *pCodec;
AVPacket		     *ffmpeg_packet;
AVFrame				 *pFrame, dst;

uint8_t			     *out_buffer;

struct SwsContext    *convert_ctx;

int					 videoindex, restart;
//enum PixelFormat	 dst_pixfmt = PIX_FMT_BGR24;
char 				 filepath[100]; // = "C:\\OPNET\\14.5.A\\models\\std\\before GT30\\traf_gen\\test.sdp";

	FFMPEGData()
	{
		// Initialize some of the values to defaults.
		restart = 1;
		videoindex = 0;
		sprintf_s(filepath, "None");
	}

};


//extern std::vector<FFMPEGData> vidData;
extern FFMPEGData* vidData;

extern int f;
extern int allocateFlag;
