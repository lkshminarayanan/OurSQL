/*
 * DataBaseServer.cpp
 *
 *  Created on: Jan 6, 2013
 *      Author: latchu
 */

#include "DataBaseServer.h"
#include <string>

using namespace datamodels;

namespace dbEngine {

DataBaseServer::DataBaseServer() {}

DataBaseServer::DataBaseServer(char* databaseName,bool createNew){
	if(createNew){
		Cache::openFile(databaseName);
	}else{
		Cache::openFile(databaseName);
	}
	SysTable = new Table(1,createNew);
	SysColumn = new Table(2,createNew);
	SysIndex = new Table(3,createNew);
}

/*
 * SysTable : tID, tName, noOfAttr, recs, startDirPage, maxColSize : 1, 2, 2, 2, 2
 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
 * SysIndex : tID, colID, startPage : 2, 2, 2
 */

DataBaseServer::~DataBaseServer(){}

int DataBaseServer::addToSysTable(char* tName, int noOfAttr, long startDirPage){
	Record *r = new Record();

	char* inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",++numOfTables);
	r->addValue(inputChar,2);

	r->addValue(tName,1);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",noOfAttr);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",0);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",startDirPage);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",100);
	r->addValue(inputChar,2);

	int attrTypeSys[6] = {2,1,2,2,2,2};

	RecordSet* rs = new RecordSet(attrTypeSys,6);
	rs->addRecord(r);

	SysTable->insertTuples(rs);

	return numOfTables;
}

int DataBaseServer::addToSysColumn(long tID, char* colName, int colID, int maxSize, int type, int isIndexed){
	//SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
	Record *r = new Record();

	char* inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",tID);
	r->addValue(inputChar,2);

	r->addValue(colName,1);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",colID);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",maxSize);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",type);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",isIndexed);
	r->addValue(inputChar,2);

	int attrTypeSys[] = {2, 1, 2, 2, 2, 2};

	RecordSet* rs = new RecordSet(attrTypeSys,6);
	rs->addRecord(r);

	SysColumn->insertTuples(rs);

	return 1;
}

int DataBaseServer::addToSysIndex(long tID, int colID, long startPage){
	// * SysIndex : tID, colID, startPage : 2, 2, 2

	char* inputChar;
	Record* r = new Record();

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",tID);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",colID);
	r->addValue(inputChar,2);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",startPage);
	r->addValue(inputChar,2);

	int attrTypeSys[] = { 2, 2, 2};

	RecordSet* rs = new RecordSet(attrTypeSys,3);
	rs->addRecord(r);

	SysIndex->insertTuples(rs);

	return 0;
}

void DataBaseServer::showTables(){
	RecordSet *rs = SysTable->selectTuples();
	rs->printAll(0,rs->getNumOfRecords());
}

} /* namespace datamodels */
