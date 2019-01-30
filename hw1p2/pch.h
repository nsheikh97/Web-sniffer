/*
 * Nabeel Sheikh
 * CSCE 463-500
 * Spring 2019
 */

 // some common .h files every .cpp needs
#pragma once
#pragma comment(lib, "Ws2_32.lib")



#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <tchar.h>
#include <WinSock2.h>
#include <Windows.h>
#include <unordered_set>
#include <string> 
#include <cmath>
#include "url.h"
#include "HTMLParserBase.h"
#include "utility.h"
#include "socket.h"

#define BOLDBLACK   "\033[1m\033[30m"
#define RESET   "\033[0m"

using namespace std;		// if you're planning to use STL
