/*utility_h
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#pragma once
#include "url.h"
#include "socket.h"

extern LARGE_INTEGER frequency; // ticks per second
extern LARGE_INTEGER t1, t2; // ticks
extern double elapsedTime;

extern struct hostent *remote;
extern struct sockaddr_in server;


int urlParse(char* url, URLStruct* newURL);
DWORD dnsLookup(URLStruct* url);
int connect(URLStruct* url, Socket& sock, const char* type);
int loading(URLStruct* url, Socket& sock, const char* type);
int verifyHeader(URLStruct* url, Socket& sock, const char* type);
int requestAndParsing(URLStruct* url, const char* type);

