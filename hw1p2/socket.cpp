/*socket_cpp
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#include "pch.h"


Socket::Socket() {
	buf = new char[INITIAL_BUF_SIZE];
	allocatedSize = INITIAL_BUF_SIZE;
	isRobot = false;
	curPos = 0;

	init();
}

Socket::Socket(const char* type) {

	buf = new char[INITIAL_BUF_SIZE];
	allocatedSize = INITIAL_BUF_SIZE;
	isRobot = false;
	curPos = 0;

	init();
	isRobot = (type == "HEAD") ? true : false;
}


void Socket::init() {

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() generated error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}
}

bool Socket::Connect(struct sockaddr_in &server) {

	// connect to the server on port

	if ((connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in))) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}


bool Socket::Read()
{
	//choosing correct max file size
	int maxFileSize = (isRobot) ? MAX_ROBOT_SIZE : MAX_FILE_SIZE;

	LARGE_INTEGER frequency; // ticks per second
	LARGE_INTEGER t1, t2; // ticks
	double elapsedTime;

	fd_set sockHolder;
	FD_ZERO(&sockHolder);
	FD_SET(sock, &sockHolder);

	// set timeout to 10 seconds 
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	while (true)
	{
		QueryPerformanceCounter(&t2);

		if ((int)ceil((t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart) > (TIMEOUT *1000))
		{
			printf("failed with slow download\n");
			return false;
		}
		int ret = 0;
		// wait to see if socket has any data
		if ((ret = select(0, &sockHolder, NULL, NULL, &timeout)) > 0)
		{
			// new data available; now read the next segment
			if (buf + curPos == 0) { break; }
			int bytes = recv(sock, (buf + curPos), (allocatedSize - curPos), 0);
			if (bytes < 0){
				printf("failed on recv: %d\n", WSAGetLastError());
				return false;
			}
			if (bytes == 0){
				return true;           
			}
			curPos += bytes;// adjust where the next recv goes 

			if (curPos >= maxFileSize) { 
				printf("failed with exceeded max\n");
				return false;
			}
			if (allocatedSize - curPos < THRESHOLD)
			{
				allocatedSize = allocatedSize * 2;
				char *reBuf = (char *)realloc(buf, allocatedSize);
				if (reBuf != NULL){
					buf = reBuf;
				}
				else{
					delete [] buf;
					printf("failed on memory allocation\n");
					return false;
				}
			}
		}
		else if(ret == 0){
			printf("failed on select timout\n");
			return false;
		}
		else {
			printf("failed at select : %d\n", WSAGetLastError());
			return false;
		}
	}
}

bool Socket::Send(char *buf)
{
	int ret = 0;
	ret = send(sock, buf, strlen(buf), 0);
	if (ret == SOCKET_ERROR) {
		printf("failed on send: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	return true;
}

char* Socket::getData() {
	buf[curPos] = '\0';
	return buf;
}

int Socket::getSize() { return curPos - 1; }

void Socket::close() {closesocket(sock);}

Socket::~Socket() {
	delete[] buf;
}

