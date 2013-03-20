/*
 * DataBaseHeader.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: latchu
 */

#include "DataBaseHeader.h"

namespace datamodels {

DataBaseHeader::DataBaseHeader(){}

DataBaseHeader::DataBaseHeader(bool newFile){
	// TODO New Page. initialize dp obj and write it to page.
	noOfTables = 0;
	noOfPages = 1;
	memcpy(p,this,sizeof(DataPage));
	writePage();
}

DataPage::DataPage(long pid):Page(pid){
	DataPage* dp;
	char* buf = new char[sizeof(DataPage)];
	memcpy(buf,p,sizeof(DataPage));
	dp = (DataPage*)buf;
	totalSlots = dp->totalSlots;
	totalRecords = dp->totalRecords;
	totalFreeSize = dp->totalFreeSize;
	contigFreeSize = dp->contigFreeSize;
	contigFreeSpaceStart = dp->contigFreeSpaceStart;
	c = p + sizeof(DataPage);
	printDetails();
}



DataPage::~DataPage() {
	// TODO Auto-generated destructor stub
	writeToPage();
	delete [] p;
}

void DataPage::writeToPage(){
	memcpy(p,this,sizeof(DataPage));
	writePage();
}


} /* namespace datamodels */
