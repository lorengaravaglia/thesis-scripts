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


#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

using namespace std;

int sleepTime = 30000;

int main(int argc, const char *argv[]) {

	// This structure will be used to create the keyboard
    // input event.
    INPUT ip;
 
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

	STARTUPINFO si, si1;
    PROCESS_INFORMATION pi, pi1;

    ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &si1, sizeof(si1) );
    si.cb = sizeof(si);
	si1.cb = sizeof(si1);
    ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &pi1, sizeof(pi1) );
	const TCHAR input[] = TEXT("ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -b:v 500k -bufsize -1000k -an -f rtp rtp://127.0.0.1:1234");
	//const TCHAR input[] = TEXT("ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 30 -an -f rtp rtp://127.0.0.1:1234");
	
	const TCHAR input1[] = TEXT("ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -crf 45 -an -f rtp rtp://127.0.0.1:1234");   //-b:v 32k -bufsize -64k
	//G:\\Master's Thesis Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.avi
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

	cout<<"first process created, sleep."<<endl;
	Sleep(sleepTime);
	cout<<"done sleeping"<<endl;
	
	for(;;)
	{

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
		
		Sleep(sleepTime);

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

		Sleep(sleepTime);

	}
    return 0;
}