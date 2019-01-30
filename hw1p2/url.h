/*url_h
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#pragma once


struct URLStruct{

	char* scheme;
	char* host;
	int port;
	char* path;
	char* query;
	char* fragment;

	URLStruct();
	URLStruct(char* schm, char* hst, int prt, char* pth, char* qry, char* frag);
	~URLStruct();
};