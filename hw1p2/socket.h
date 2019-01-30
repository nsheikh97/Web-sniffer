/*socket_h
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#pragma once

#include "pch.h"

#define INITIAL_BUF_SIZE 8192
#define THRESHOLD 512
#define TIMEOUT 10
#define MAX_ROBOT_SIZE 16384
#define MAX_FILE_SIZE 2097152;

class Socket{
private:
	SOCKET sock;
	char *buf;
	int allocatedSize;
	int curPos;
	bool isRobot;
public:
	Socket();
	Socket(const char*);
	void init();
	bool Connect(struct sockaddr_in &server);
	bool Read();
	bool Send(char *buf);
	void close();
	char* getData();
	int  getSize();
	~Socket();
};