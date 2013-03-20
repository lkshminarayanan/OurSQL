/*
 * Cache.h
 *
 *  Created on: Dec 13, 2012
 *      Author: latchu
 */

#ifndef CACHE_H_

#define CACHE_H_
#include <fstream>
#include <iostream>
# include <vector>
#include<string.h>
#include<string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include "global.h"

#define pageSize 1024*4


using namespace std;

namespace datamodels {


class Cache {
private:
	fstream db;
	long pageNum;
//	string DBName;
	static Cache* cacheInstance;
	Cache();

public:

	static Cache* getInstance();
	static int openFile(char* filename);
	virtual ~Cache();

	int getNewPage();

	char* readPage(long);
	int writePage(long pageid, char* page);

	void readAll();
	bool isDBExists(string);

	long getPageNum();


};


} /* namespace datamodels */
#endif /* CACHE_H_ */
