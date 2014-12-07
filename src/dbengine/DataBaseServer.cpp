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
	dbh = new DataBaseHeader(databaseName,createNew);
	if(dbh->getNoOfPages()==-1){
		valid = false;
		return;
	}
	valid = true;
	SysTable = new Table(1,createNew);
	SysColumn = new Table(2,createNew);
	SysIndex = new Table(3,createNew);
	numOfTables = dbh->getNoOfTables();
}


/*
 * SysTable : tID, tName, noOfAttr, recs, startDirPage, maxColSize : 2, 1, 2, 2, 2, 2
 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
 * SysIndex : tID, colID, startPage : 2, 2, 2
 */

DataBaseServer::~DataBaseServer(){
	delete dbh;
}

bool DataBaseServer::isValid(){
	return valid;
}

int DataBaseServer::addToSysTable(char* tName, int noOfAttr, long startDirPage){
	Record *r = new Record();

	char* inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",++numOfTables);
	r->addValue(inputChar,2);
	dbh->updateTableCount();

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

	dbh->writeToPage();

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

	dbh->writeToPage();

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

	dbh->writeToPage();

	return 0;
}

void DataBaseServer::showTables(){
	RecordSet *rs;
	int pos[] = {1};
	int tAttr[] = {2,1,2,2,2,2};
	//Select *select = new Select(pos,1,tAttr,6);
	rs = SysTable->selectTuples(new Select(),new Where());
	//rs = SysColumn->selectTuples();
	int n = rs->getNumOfRecords();
	int i;
	if(n>0){
		//rs->printAll(0,rs->getNumOfRecords());
		vector<Record*> tables = rs->getAllRecords();
		for(i=0;i<n;i++){
			cout <<endl<<tables[i]->getValues()[1];
		}
		rs->printAll(0,rs->getNumOfRecords());
		cout << endl;
	}else
		cout<<"No tables found."<<endl;
}

} /* namespace datamodels */
