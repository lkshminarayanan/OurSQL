/*
 * DataBaseHeader.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: latchu
 */

#include "DataBaseHeader.h"

namespace datamodels {

DataBaseHeader::DataBaseHeader(){}

DataBaseHeader::DataBaseHeader(char* fileName, bool newFile){
	// TODO New Page. initialize dp obj and write it to page.
	if(newFile){
		noOfTables = 0;
		noOfPages = 1;
		//start the cache;
		int stat = Cache::createCache(fileName,newFile);
		if(stat == -1)
			noOfPages = -1;
		else{
			writeToPage();
		}
	}else{
		int stat = Cache::createCache(fileName,newFile);
		if(stat == -1)
			noOfPages = -1;
		else{
			Cache *c = Cache::getInstance();
			char *p = c->readPage(0);
			DataBaseHeader *dbh = (DataBaseHeader*)p;
			noOfTables = dbh->noOfTables;
			noOfPages = dbh->noOfPages;
			c->setPageNum(noOfPages);
		}
	}
	if(noOfPages != -1){
		lg("@DBHeader : DB Loaded. Pages-"<<noOfPages<<". Tables-"<<noOfTables);
	}

}

DataBaseHeader::~DataBaseHeader() {
	// TODO Auto-generated destructor stub
	writeToPage();
}

long DataBaseHeader::getNoOfPages(){
	return noOfPages;
}

long DataBaseHeader::getNoOfTables(){
	return noOfTables;
}

void DataBaseHeader::updateTableCount(){
	noOfTables ++;
}

void DataBaseHeader::writeToPage(){
	char* p = new char[pageSize];
	Cache *cache = Cache::getInstance();
	noOfPages = cache->getPageNum();
	memcpy(p,this,sizeof(DataBaseHeader));
	lg("Updating DBHeader : PgNo -"<<noOfPages<<" TblNo"<<noOfTables);
	cache->writePage(0,p);
}


} /* namespace datamodels */

//using namespace datamodels;
//int main(){
//	DataBaseHeader *db = new DataBaseHeader("Temp",false);
//	cout << cout<<db->getNoOfPages();
//	db->updateTableCount();
//	db->writeToPage();
//}

