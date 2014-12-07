/*
 * DataBaseHeader.h
 *
 *  Created on: Mar 20, 2013
 *      Author: latchu
 */

#ifndef DATABASEHEADER_H_
#define DATABASEHEADER_H_
#include <iostream>
#include "Cache.h"
#include <string.h>
#include "global.h"
using namespace std;

namespace datamodels {

class DataBaseHeader{
	long noOfTables;
	long noOfPages;
public:
	DataBaseHeader();
	DataBaseHeader(char* fileName, bool newFile);

	virtual ~DataBaseHeader();

	long getNoOfTables();
	long getNoOfPages();

	void updateTableCount();

	void writeToPage();
};

} /* namespace datamodels */
#endif /* DATABASEHEADER_H_ */
