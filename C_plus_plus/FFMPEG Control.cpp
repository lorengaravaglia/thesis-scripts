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

	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;

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
	
	TCHAR input2[1024];

	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	
	//while (TRUE)
	//{
	if (!CreateProcess(NULL,   // No module name (use command line)
			(LPWSTR)input,        // Command line  (LPWSTR)input
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return 0;
		}
		
		while (hPipe != INVALID_HANDLE_VALUE)
		{
			printf("waiting for named pipe\n");
			if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
			{
				printf("connect pipe != to false\n");
				//while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
				while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
				{
					// add terminating zero
					buffer[dwRead] = '\0';
					printf("got data from named pipe, checking the value.\n");
					if (strcmp(buffer, "1") == 0)
					{
						printf("data from named pipe did match string comparison.\n");
						TerminateProcess(pi.hProcess, NULL);
						int a = rand() % 2;

						if (a > 0)
						{
							_tcscpy_s(input2, input);
						}
						else
						{
							_tcscpy_s(input2, input1);
						}
						if (!CreateProcess(NULL,   // No module name (use command line)
							(LPWSTR)input2,        // Command line  (LPWSTR)input
							NULL,           // Process handle not inheritable
							NULL,           // Thread handle not inheritable
							FALSE,          // Set handle inheritance to FALSE
							0,              // No creation flags
							NULL,           // Use parent's environment block
							NULL,           // Use parent's starting directory 
							&si,            // Pointer to STARTUPINFO structure
							&pi)           // Pointer to PROCESS_INFORMATION structure
							)
						{
							printf("CreateProcess failed (%d).\n", GetLastError());
							return 0;
						}
					}
					// do something with data in buffer 
					printf("%s\n", buffer);
				}
			}	
		}
	//}

	DisconnectNamedPipe(hPipe);
    return 0;
}