/*
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#include "pch.h"

//time objects using https://stackoverflow.com/questions/2150291/how-do-i-measure-a-time-interval-in-c
LARGE_INTEGER frequency = { 0 }; // ticks per second
LARGE_INTEGER t1 = { 0 }, t2 = { 0 }; // ticks
double elapsedTime = 0;

// structure used in DNS lookups
struct hostent *remote;

// structure for connecting to server
struct sockaddr_in server;

int urlParse(char* url, URLStruct* newURL) {
	
	// scheme://host[:port][/path][?query][#fragment] 

	char* scheme = new char[512];
	char* host = new char[MAX_HOST_LEN];
	char* prt = new char[512];
	char* path = new char[MAX_REQUEST_LEN];
	char* query = new char[MAX_REQUEST_LEN];
	char* fragment = new char[MAX_REQUEST_LEN];
	
	int port = 80;
	int cursor = 0;
	int tCursor = 0;

	bool hasPath = false;
	bool hasQuery = false;
	bool hasFragment = false;


	//getting scheme
	while (url[cursor] != ':') {

		scheme[tCursor] = url[cursor];
		cursor++;
		tCursor++;
	}
	scheme[tCursor] = '\0';
	tCursor = 0;
	//skip '//'
	cursor += 3;
	if (strcmp(scheme, "http") != 0) {
		printf("failed with invalid scheme");
		return -1;
	}

	//getting host
	while (url[cursor] != '\0' && url[cursor] != '/' && url[cursor] != '?' && url[cursor] != '#' && url[cursor] != ':') {
		host[tCursor] = url[cursor];
		cursor++;
		tCursor++;
	}
	host[tCursor] = '\0';
	tCursor = 0;
	
	//check for port
	if (url[cursor] == ':') {
		cursor++;
		while (url[cursor] != '\0' && url[cursor] != '/' && url[cursor] != '?' && url[cursor] != '#') {
			prt[tCursor] = url[cursor];
			cursor++;
			tCursor++;
		}
		prt[tCursor] = '\0';
		tCursor = 0;

		port = atoi(prt);
		if (port == 0) {
			printf("failed with invalid port");
			return -1;
		}
	}

	//check for path
	if (url[cursor] == '/') {
		
		while (url[cursor] != '\0' && url[cursor] != '?' && url[cursor] != '#') {
			path[tCursor] = url[cursor];
			cursor++;
			tCursor++;
		}
		path[tCursor] = '\0';
		tCursor = 0;
		hasPath = true;
	}
	//check for query
	if (url[cursor] == '?') {
		
		while (url[cursor] != '\0' && url[cursor] != '#') {
			query[tCursor] = url[cursor];
			cursor++;
			tCursor++;
		}
		query[tCursor] = '\0';
		tCursor = 0;
		hasQuery = true;
	}
	//check for fragment
	if (url[cursor] == '#') {
		
		while (url[cursor] != '\0') {
			fragment[tCursor] = url[cursor];
			cursor++;
			tCursor++;
		}
		fragment[tCursor] = '\0';
		tCursor = 0;
		hasFragment = true;
	}

	//assigning default values if no path, query or fragment was parsed.
	if (!hasPath) strcpy_s(path, sizeof path, "/");
	if (!hasQuery) strcpy_s(query, sizeof query, "");
	if (!hasFragment) strcpy_s(fragment, sizeof fragment, "");

	//Assign parsed fields to URL struct
	newURL->scheme = scheme;
	newURL->host = host;
	newURL->port = port;
	newURL->path = path;
	newURL->query = query;
	newURL->fragment = fragment;

	//usage info printed to console
	printf(" host %s, port %d\n", host, port);

	delete[] prt;

}

DWORD dnsLookup(URLStruct* url) {
	printf("	Doing DNS...");

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	// first assume that the string is an IP address
	DWORD IP = inet_addr(url->host);
	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(url->host)) == NULL)
		{
			printf("failed on invalid host \n");
			return INADDR_NONE;
		}
		else // take the first IP address and copy into sin_addr
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
			
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
	}
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	printf("done in %d ms", (int)elapsedTime);
	printf(" Found %s \n", inet_ntoa(server.sin_addr));
	return inet_addr(inet_ntoa(server.sin_addr));
}

int connect(URLStruct* url, Socket& sock, const char* type) {
	
	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons(url->port);		// host-to-network flips the byte order

	//connecting to socket
	QueryPerformanceCounter(&t1);
	if(type == "HEAD") printf("        Connecting on robots...");
	else printf("       *Connecting on page...");
	if (!sock.Connect(server)) {
		printf("failed with %d\n", WSAGetLastError());
		return -1;
	}
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	printf("done in %d ms\n", (int)elapsedTime);

	return 1;
}

int loading(URLStruct* url, Socket& sock, const char* type) {

	char * req = new char[1024];
	if (type == "HEAD") {
		//HEAD request
		sprintf(req, "HEAD /robots.txt HTTP/1.0\r\nUser-Agent: webSniffer/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url->host);
	}
	else {
		//GET request
		sprintf(req, "GET %s%s HTTP/1.0\r\nUser-Agent: webSniffer/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", url->path, url->query, url->host);
	}
	//send request
	if (!sock.Send(req)) {
		delete[] req;
		return -1;
	}

	//loading 
	QueryPerformanceCounter(&t1);
	printf("	Loading...");
	if (!sock.Read()) {
		delete[] req;
		return -1;
	}
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	printf("done in %d ms with %d bytes\n", (int)elapsedTime, sock.getSize());

	delete[] req;
	return 1;
}

int verifyHeader(URLStruct* url, Socket& sock, const char* type) {

	char* data;
	printf("	Verifying Header...");

	if ((data = strstr(sock.getData(), "HTTP/")) == NULL) {
		printf("failed with non-HTTP header");
		return -1;
	}

	//move cursor to version
	data += 9;

	//2xx\0
	char statCode[4];
	memcpy(statCode, data, 3);
	statCode[3] = '\0';
	
	printf("status code %s\n", statCode);

	int code =  atoi(statCode);

	//4xx
	if (type == "HEAD") {
		if (code >= 400 && code < 500) return code;
		else return -1;
	}
	//2xx
	else if (type == "GET") {
		if (code >= 200 && code < 300) return code;
		else return -1;
	}
	else return -1;
}

void HTMLParsing(URLStruct* url, Socket& sock) {
	QueryPerformanceCounter(&t1);
	printf("       +Parsing page...");
	HTMLParserBase *parser = new HTMLParserBase;

	char baseUrl[MAX_REQUEST_LEN];		// where this page came from; needed for construction of relative links

	//construct base URL
	sprintf(baseUrl, "%s://%s\0", url->scheme, url->host);

	int nLinks;
	char *linkBuffer = parser->Parse(sock.getData(), strlen(sock.getData()), baseUrl, (int)strlen(baseUrl), &nLinks);

	// check for errors indicated by negative values
	if (nLinks < 0) nLinks = 0;

	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	printf("done in %d ms with %d links\n", (int)elapsedTime, nLinks);


	delete parser;		// this internally deletes linkBuffer
}


int requestAndParsing(URLStruct* url, const char* type) {

	//create sock
	Socket sock(type);

	//connecting to socket
	if (connect(url, sock, type) < 0) return -1;

	//loading data based on request
	if (loading(url, sock, type) < 0) return -1;

	//verifying header
	if (verifyHeader(url, sock, type) < 0) return -1;

	//parse HTML only if robots.txt does not exist
	if (type == "GET") HTMLParsing(url, sock);

	//close sock
	sock.close();
}


