/*
 * Parser.h
 *
 *  Created on: Apr 17, 2013
 *      Author: latchu
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "dbengine/Table.h"
#include "dirent.h"
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdexcept>
#include <stdio.h>

#define cmderror cout<<"Please check your query syntax.(Debugging : Line-"<<__LINE__<<")"<<endl

namespace dbEngine {

class Parser {
private:
	DataBaseServer *dbs;
public:
	Parser();
	int parse(string stmt);
	void actUpon(int ch,vector<string> token);
	bool isAlpha(string s);
	bool isInt(string s);
	bool isReal(string s);
	string findType(string s);
	int str2int (string s);
	Where *prepareWhere(vector<string> tokens, vector<string> colNames, vector<int> colTypes, unsigned i);
	void loadFromFile(string fileName);
};

// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}


} /* namespace datamodels */
#endif /* PARSER_H_ */

