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
#include <vector>
#include "dbengine/Table.h"
#include "dirent.h"

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
};

} /* namespace datamodels */
#endif /* PARSER_H_ */

