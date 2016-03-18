#define __STDC_CONSTANT_MACROS
#include <stdio.h> 
#include <tchar.h>
extern "C" 
{
        // NOTE: Additional directory ..\zeranoe.com\dev\include gets to the files
        #include "libavcodec\avcodec.h"
		#include "libavformat\avformat.h"
		#include "libavutil\dict.h"
}

// NOTE: Additional directory ..\zeranoe.com\dev\lib gets to the files
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
// NOTE: Be sure to copy DLL files from ..\zeranoe.com\shared\bin to the directory of 
//       the FFmpegApp.exe binary
int _tmain(int argc, _TCHAR* argv[])
{
	av_register_all();
	const char    *url = "G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi";
	AVFormatContext *s = NULL;
	//int ret = avformat_open_input(&s, url, NULL, NULL);
	//if (ret < 0)
	//{
	//	 abort();
	//}
	//printf("opened input\n");
	AVDictionary *options = NULL;
	av_dict_set(&options, "video_size", "640x480", 0);
	av_dict_set(&options, "pixel_format", "rgb24", 0);
	if (avformat_open_input(&s, url, NULL, &options) < 0)
	{
		abort();
	}
	printf("setup dictionary\n");
	av_dict_free(&options);
	printf("freed dictionary\n");
	AVDictionaryEntry *e;
	if (e = av_dict_get(options, "", NULL, AV_DICT_IGNORE_SUFFIX))
	{
		fprintf(stderr, "Option %s not recognized by the demuxer.\n", e->key);
		abort();
	}
}