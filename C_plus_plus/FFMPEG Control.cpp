#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>
#include <assert.h>
#include <vector>

using namespace std;

int sleepTime = 30000;
int nodeNumber = 10;

struct processes 
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	processes()
	{
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
	}
};

char** str_split(char* a_str, const char a_delim)
{
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	char *next_token = NULL;

	/* Count how many elements will be extracted. */
	while (*tmp)
	{
		if (a_delim == *tmp)
		{
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += last_comma < (a_str + strlen(a_str) - 1);

	/* Add space for terminating null string so caller
	knows where the list of returned strings ends. */
	count++;

	result = (char**)malloc(sizeof(char*) * count);

	if (result)
	{
		size_t idx = 0;
		char* token = strtok_s(a_str, delim, &next_token);

		while (token)
		{
			assert(idx < count);
			*(result + idx++) = _strdup(token);
			token = strtok_s(0, delim, &next_token);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}

	return result;
}

int main(int argc, const char *argv[]) {

	char** tokens;
	char   bitrate[10];
	char   node[4];

	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;

	vector<processes> proc;

	wchar_t command[250]; //= L"ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -b:v 500k -bufsize -1000k -an -f rtp rtp://127.0.0.1:1234";
	//const TCHAR input1[] = TEXT("ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -b:v 20k -bufsize -40k -an -f rtp rtp://127.0.0.1:1235");   //-b:v 32k -bufsize -64k
	
	// Initialze the Named Pipe.
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);


	// Initialize the FFMPEG processes for all nodes.
	for (int i = 0; i < nodeNumber; i++)
	{
		// Create a struct entry for every node.
		proc.push_back(processes());

		swprintf_s(command, sizeof(command), L"ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -b:v %dk -bufsize -1000k -an -f rtp rtp://127.0.0.1:12%d", 500, (10+i));
		
		if (!CreateProcess(NULL,    // No module name (use command line)
			(LPWSTR)command,        // Command line  (LPWSTR)input
			NULL,                   // Process handle not inheritable
			NULL,                   // Thread handle not inheritable
			FALSE,                  // Set handle inheritance to FALSE
			0,                      // No creation flags
			NULL,                   // Use parent's environment block
			NULL,                   // Use parent's starting directory 
			&proc[i].si,            // Pointer to STARTUPINFO structure
			&proc[i].pi)            // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return 0;
		}
	}

	// Keep looping while the pipe is valid.
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		printf("waiting for named pipe.\n");

		// Wait for someone to connect to the pipe.
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   
		{
			printf("Someone connected to the pipe.\n");
			
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				// Add terminating zero
				buffer[dwRead] = '\0';

				// Split the input string into two parts, node number and new bitrate.
				tokens = str_split(buffer, ',');
				
				if (tokens)
				{
					strcpy_s(node, *(tokens));
					printf("node = %s\n", node);
					strcpy_s(bitrate, *(tokens + 1));
					printf("bitrate = %s\n", bitrate);
				}

				free(tokens);

				printf("got data from named pipe, checking the node.\n");

				// Convert the node number to int for easier comparison.
				// Subtract 1 from the value to match the range of the vector.
				int n = atoi(node) - 1;

				//if (strcmp(node, "1") == 0)
				//{

				// Is this a valid node?
				if(n < nodeNumber)
				{
					printf("Node is in range.\n");
						
					// Create the string associated with each node.  The node number is used to select a port number.
					swprintf_s(command, sizeof(command), L"ffmpeg -f concat -re -i G:\\Masters_Thesis_Files\\Honda_Database\\Database1\\Training\\videos\\behzad\\behzad.txt -vcodec libx264 -g 10 -vf format=gray -b:v %hsk -bufsize -1000k -an -f rtp rtp://127.0.0.1:12%d", bitrate, (10 + n));
					
					TerminateProcess(proc[n].pi.hProcess, NULL);

					if (!CreateProcess(NULL,    // No module name (use command line)
						(LPWSTR)command,        // Command line  (LPWSTR)input
						NULL,                   // Process handle not inheritable
						NULL,                   // Thread handle not inheritable
						FALSE,                  // Set handle inheritance to FALSE
						0,                      // No creation flags
						NULL,                   // Use parent's environment block
						NULL,                   // Use parent's starting directory 
						&proc[n].si,        // Pointer to STARTUPINFO structure
						&proc[n].pi)        // Pointer to PROCESS_INFORMATION structure
						)
					{
						printf("CreateProcess failed (%d).\n", GetLastError());
						return 0;
					}
				}
				else
				{
					printf("Node not in range.\n");
				}

				// Do something with data in buffer. 
				printf("%s\n", buffer);
			}
		}	
	}
	DisconnectNamedPipe(hPipe);
    return 0;
}