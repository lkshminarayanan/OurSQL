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
		noOfFreePages = 0;
		freePages = NULL;
		dbPageSize = pageSize;
		//start the cache;
		int stat = Cache::createCache(fileName,newFile);
		if(stat == -1)
			noOfPages = -1;
		else{
			Cache::getInstance()->setFreePages(freePages,0);
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
			dbPageSize = dbh->dbPageSize;
			noOfFreePages = 0;//dbh->noOfFreePages;
			/*if(noOfFreePages>0){
				freePages = new long[noOfFreePages];
				memcpy(freePages,p+sizeof(DataBaseHeader),noOfFreePages*sizeof(long));
			}else{
				freePages = NULL;
			}*/
			freePages = NULL;
			c->setFreePages(freePages,noOfFreePages);
			c->setPageNum(noOfPages);

			delete [] p;
		}

	}
	lg("@DBHeader : DB Loaded. Pages-"<<noOfPages<<". Tables-"<<noOfTables);
	//logDetails();
}

DataBaseHeader::~DataBaseHeader() {
	// TODO Auto-generated destructor stub
	if(pageSize==this->dbPageSize) writeToPage();
	Cache *cache = Cache::getInstance();
	delete cache;
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
	memset( p, '\0', sizeof(char)*pageSize );
	Cache *cache = Cache::getInstance();
	noOfPages = cache->getPageNum();
	noOfFreePages = 0;//cache->getCountFreePages();
	memcpy(p,this,sizeof(DataBaseHeader));
	if(noOfFreePages>0){
		freePages = cache->getFreePages();
		memcpy(p+sizeof(DataBaseHeader),freePages,noOfFreePages*sizeof(long));
	}

	lg("Updating DBHeader : PgNo -"<<noOfPages<<" TblNo"<<noOfTables);
	cache->writePage(0,p);
	delete[] p;
}

void DataBaseHeader::logDetails(){
	if(noOfPages != -1){
		cout<<"DB Log.\n Pages-"<<noOfPages<<".\n Tables-"<<noOfTables;
		cout<<"\n noOfFreePages-"<<noOfFreePages<<endl;
		for(int i=0;i<noOfFreePages;i++){
			cout << freePages[i]<<" ";
		}
	}
}

long DataBaseHeader::getDBPageSize(){
	return dbPageSize;
}


} /* namespace datamodels */

//using namespace datamodels;
//int main(){
//	DataBaseHeader *db = new DataBaseHeader("Temp",false);
//	cout << cout<<db->getNoOfPages();
//	db->updateTableCount();
//	db->writeToPage();
//}

