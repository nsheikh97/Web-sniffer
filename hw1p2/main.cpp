/*main_cpp
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#include "pch.h"


int main(int argc, char** argv) {

	WSADATA wsaData;

	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//checking for correct user input
	if (argc == 2) {

		char curURL[1024];
		memcpy(curURL, argv[1], strlen(argv[1]) + 1);

		//printing URL to console
		printf_s("URL: %s \n", curURL);
		//parse URL ****************
		printf("	Parsing URL...");
		URLStruct* newURL = new URLStruct();
		urlParse(curURL, newURL);
		//**************************

		DWORD ip = dnsLookup(newURL);
		if (ip == INADDR_NONE) {
			return -1;
		}

		if (requestAndParsing(newURL, "GET") < 0) {
			delete[] newURL;
			return -1;
		}
		delete[] newURL;
		return 1;
	}
	else if (argc == 3) {
		if (atoi(argv[1]) != 1) {
			printf("Usage: \n\t.exe <# of threads> <url text file>\n");
			return -1;
		}
	}
	else if (argc != 3) {
		printf("Usage: \n\t.exe <# of threads> <url text file>\n");
		return -1;
	}

	//creating unordered sets for host and ip uniqueness tests
	unordered_set<string>* seenHosts = new unordered_set<string>; //hosts
	unordered_set<DWORD>* seenIPs = new unordered_set<DWORD>; //IPs

	//retrieving url from argument passed by user
	char textFile[1024];
	//copy argument to url
	memcpy(textFile, argv[2], strlen(argv[2]) + 1);



	//Reading file --------------
	FILE* pFile;
	long fileSize;
	char* fileBuf;
	size_t result;

	pFile = fopen(textFile, "rb");
	if (pFile == NULL) {
		printf("Error opening file...Exiting");
		return -1;
	}

	//getting file size:
	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);

	// allocate memory based on file size
	fileBuf = (char*)malloc(sizeof(char)*fileSize+1);
	if (fileBuf == NULL) { 
		printf("Memory Error...Exiting");
		return -1;
	}

	//ending buffer with null
	fileBuf[fileSize] = NULL;

	// copy the file into buffer:
	result = fread(fileBuf, 1, fileSize, pFile);
	if (result != fileSize) {
		printf("File read Error...Exiting");
		return -1;
	}


	//file loaded into buffer
	printf("Opened %s with size %d \n", textFile, fileSize);

	// close file
	fclose(pFile);

	//--------------------------------

	//url line by line ---------------
	int cursor = 0; //filBuf cursor
	int lineCursor = 0; //cursor for a single line
	while (cursor != fileSize + 1) {

		char curURL[MAX_URL_LEN + 1]; //plus one for NULL character
		bool isExceeded = false; //check for overflow


		if (lineCursor > MAX_URL_LEN) {

			printf("URL length exceeded %d bytes", MAX_URL_LEN);
			isExceeded = true;
		}
		//found url and no url buffer overflow
		else if ((fileBuf[cursor] == '\n' || fileBuf[cursor] == '\r') && isExceeded == false) {

			curURL[lineCursor] = '\0';

			//due to line ending differences, checking if curURL contains a single \r or \n
			if (strlen(curURL) == 0) {

				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}


			//do the things --------------------

			//printing URL to console
			printf_s("URL: %s \n", curURL);
			
			//parse URL ****************
			printf("	Parsing URL...");
			URLStruct* newURL = new URLStruct();
			urlParse(curURL, newURL);
			//**************************

			//check host uniqueness*****
			int prevSize = seenHosts->size();
			seenHosts->insert(newURL->host);
			printf("\tChecking host uniqueness...");
			if (seenHosts->size() > prevSize) {
				printf("passed\n");
			}
			else {
				printf("failed\n");

				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}

			//**************************

			//dns lookup****************
			DWORD ip = dnsLookup(newURL);
			if (ip == INADDR_NONE) {
				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}
			//**************************

			//check ip uniqueness*****
			prevSize = seenIPs->size();
			seenIPs->insert(ip);
			printf("\tChecking IP uniqueness...");
			if (seenIPs->size() > prevSize) printf("passed\n");
			else {
				printf("failed\n");

				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}

			//requestAndPArsing for robots*********
			if (requestAndParsing(newURL, "HEAD") < 0) {
				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}


			//parse HTML if robots was not found***
			if(requestAndParsing(newURL, "GET") < 0) {
				//continue to next iteration
				lineCursor = 0; //reset lineCursor
				cursor++; //continue to next line 
				continue;
			}

			//**************************************

			//cleanup*******************
			delete newURL;
			//**************************
			//-----------------------------------

			//reset lineCursor
			lineCursor = 0;
			//continue to next line
			cursor++;
		}

		//if exceeded url max length and at end of line, continue to next line
		else if (isExceeded == true && (fileBuf[cursor] == '\n' || fileBuf[cursor] == '\r')) {
			//reset lineCursor
			lineCursor = 0;
			//continue to next line
			cursor++;
			//reset flag
			isExceeded = false;
		}
		//if url buffer is full and in middle of line, continue until end of line is hit
		else if (isExceeded == true) {

			cursor++;
		}
		else {
			//add current char to line url and increment cursors
			if(curURL[lineCursor] = fileBuf[cursor]);
			lineCursor++;
			cursor++;
		}
	}

	//cleanup
	delete [] fileBuf;
	delete seenHosts;
	delete seenIPs;
	WSACleanup();

	return 0;
}