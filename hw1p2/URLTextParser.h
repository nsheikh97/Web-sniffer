#pragma once

#include <fstream>
#include <string>

class URLTextParser {
private:
	size_t size;
	const char* textFile;
public:

	URLTextParser(const char* textfile) : textFile(textFile);

	char* getLine(const char* textFile) {

		std::string line;
		std::ifstream myfile(textFile);
		if (myfile)  // same as: if (myfile.good())
		{
			(std::getline(myfile, line));  // same as: while (getline( myfile, line ).good())

			char* toReturn = new char[line.length() + 1];
			strcpy(toReturn, line.c_str());

			myfile.close();

			return toReturn;
		}
		else
			printf("Error opening file: File does not exist");


	}
};