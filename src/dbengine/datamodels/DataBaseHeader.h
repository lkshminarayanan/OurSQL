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
	long noOfFreePages;
	long* freePages;
	long dbPageSize;
public:
	DataBaseHeader();
	DataBaseHeader(char* fileName, bool newFile);

	virtual ~DataBaseHeader();

	long getNoOfTables();
	long getNoOfPages();
	long getDBPageSize();

	void updateTableCount();

	void writeToPage();

	void logDetails();
};

} /* namespace datamodels */
#endif /* DATABASEHEADER_H_ */
