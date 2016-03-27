/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

/*
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>

//using namespace cv;
using namespace std;

//VideoCapture cap;
#define BUFSIZE 512

clock_t init;

int main(int argc, const char *argv[]) {

	STARTUPINFO si, si1;
    PROCESS_INFORMATION pi, pi1;

    ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &si1, sizeof(si1) );
    si.cb = sizeof(si);
	si1.cb = sizeof(si1);
    ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &pi1, sizeof(pi1) );
	const TCHAR input[] = TEXT("ffmpeg -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi -vcodec libx264 -vf format=gray -g 30 -an -f rtp rtp://127.0.0.1:1234");
	//const TCHAR input[] = TEXT("ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 30 -an -f rtp rtp://127.0.0.1:1234");
	const TCHAR input1[] = TEXT("ffmpeg -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi -vcodec libx264 -vf format=gray -g 30 -crf 40 -an -f rtp rtp://127.0.0.1:1234");
	//G:\\Master's Thesis Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi
	//F:\\Downloads\\jap.avi
    // Start the child process. 



	if( !CreateProcess( NULL,   // No module name (use command line)
        (LPWSTR)input,        // Command line  (LPWSTR)input
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return 0;
    }

	cout<<"first process created, sleep for 20 seconds"<<endl;
	Sleep(20000);
	cout<<"done sleeping"<<endl;
	int j = 1;

    // Get a handle to the Video device:
    
	/*
	cap.open("C:\\Users\\Loren\\Documents\\Visual Studio 2012\\Projects\\OpencvSetup\\OpencvSetup\\test.sdp");
	cout<<"stream opened"<<endl;
    // Check if we can use this device at all:
    if(!cap.isOpened()) {
        cerr << "Capture Device ID cannot be opened." << endl;
        return -1;
    }
	*/

    // Holds the current frame from the Video device:
    //Mat frame;
    
	for(;;) {
		//cap >> frame;

		//if(j%6600 == 0)
		//{

		TerminateProcess(pi1.hProcess, NULL);

			CreateProcess( NULL,   // No module name (use command line)
						  (LPWSTR)input,        // Command line
						  NULL,           // Process handle not inheritable
						  NULL,           // Thread handle not inheritable
						  FALSE,          // Set handle inheritance to FALSE
						  0,              // No creation flags
						  NULL,           // Use parent's environment block
						  NULL,           // Use parent's starting directory 
						  &si,            // Pointer to STARTUPINFO structure
						  &pi);
			Sleep(30000);


			//cap.open("C:\\Users\\Loren\\Documents\\Visual Studio 2012\\Projects\\OpencvSetup\\OpencvSetup\\test.sdp");
			//cout<<"stream opened"<<endl;
		//}
		//else if(j %3300 == 0)
		//{
			TerminateProcess(pi.hProcess, NULL);

			CreateProcess( NULL,   // No module name (use command line)
						  (LPWSTR)input1,        // Command line
						  NULL,           // Process handle not inheritable
						  NULL,           // Thread handle not inheritable
						  FALSE,          // Set handle inheritance to FALSE
						  0,              // No creation flags
						  NULL,           // Use parent's environment block
						  NULL,           // Use parent's starting directory 
						  &si1,            // Pointer to STARTUPINFO structure
						  &pi1);
			Sleep(30000);
			//cap.open("C:\\Users\\Loren\\Documents\\Visual Studio 2012\\Projects\\OpencvSetup\\OpencvSetup\\test.sdp");
			//cout<<"stream opened"<<endl;
			//j = 0;
		//}
		/*
		//int bytes = frame.total()*frame.elemSize();
		//cout<<"size = "<<bytes<<endl;
		// Show the result:
	    cv::imshow("face_recognizer", frame);
		// And display it:
	    char key = (char) waitKey(20);
		// Exit this loop on escape:
		if(key == 27)
			break;
		*/
		j++;
		cout<<j<<endl;
		}
    return 0;
}