/*url_cpp
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

#include "pch.h"

//constructor
URLStruct::URLStruct(char* schm, char* hst, int prt, char* pth, char* qry, char* frag) : 
	scheme(schm), host(hst), port(prt), path(pth), query(qry),fragment(frag) {};

//defualt constructor
URLStruct::URLStruct() {}

//deconstructor
URLStruct::~URLStruct() {
	delete[] scheme;
	delete[] host;
	delete[] path;
	delete[] query;
	delete[] fragment;
}