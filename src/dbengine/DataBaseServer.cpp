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

char DataBaseServer::dataDirectory[FILENAME_MAX];

DataBaseServer::DataBaseServer() {}

DataBaseServer::DataBaseServer(char* databaseName,bool createNew){

	dbName = new char[strlen(databaseName)];
	strcpy(dbName, databaseName);

	char completeDatabaseFilePath[FILENAME_MAX];
	strcpy(completeDatabaseFilePath, dataDirectory);
	strcat(completeDatabaseFilePath, databaseName);
	dbh = new DataBaseHeader(completeDatabaseFilePath,createNew);
	if(dbh->getNoOfPages()==-1){
		valid = 0;
		return;
	}

	else if(dbh->getDBPageSize()!=pageSize){
		cout << "Error : PageSize Mismatch.(actual :"<<dbh->getDBPageSize()<<" | system :"<<pageSize<<")\n";
		valid = -1;
		return;
	}
	valid = 1;
	SysTable = new Table(1,createNew);
	SysColumn = new Table(2,createNew);
	SysIndex = new Table(3,createNew);
	numOfTables = dbh->getNoOfTables();
}


/*
 * SysTable : tID, tName, noOfAttr, recs, startDirPage, maxColSize : 2, 1, 2, 2, 2, 2
 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
 * SysIndex : tID, colID, startPage, indexName : 2, 2, 2, 1
 */

DataBaseServer::~DataBaseServer(){
	delete[] dbName;
	if(valid>0){
		delete SysTable;
		delete SysColumn;
		delete SysIndex;
	}
	delete dbh;
}

int DataBaseServer::isValid(){
	return valid;
}

int DataBaseServer::addToSysTable(char* tName, int noOfAttr, long startDirPage){
	Record *r = new Record();

	char* inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",++numOfTables);
	r->addValue(inputChar,2);
	dbh->updateTableCount();
	delete[] inputChar;

	r->addValue(tName,1);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",noOfAttr);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",0);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",startDirPage);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",100);
	r->addValue(inputChar,2);
	delete[] inputChar;

	int attrTypeSys[6] = {2,1,2,2,2,2};

	RecordSet* rs = new RecordSet(attrTypeSys,6);
	rs->addRecord(r);

	SysTable->insertTuples(rs);

	delete rs;

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
	delete[] inputChar;

	r->addValue(colName,1);

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",colID);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",maxSize);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",type);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",isIndexed);
	r->addValue(inputChar,2);
	delete[] inputChar;

	int attrTypeSys[] = {2, 1, 2, 2, 2, 2};

	RecordSet* rs = new RecordSet(attrTypeSys,6);
	rs->addRecord(r);

	SysColumn->insertTuples(rs);

	delete rs;

	dbh->writeToPage();

	return 1;
}

int DataBaseServer::addToSysIndex(long tID, int colID, long startPage, char* indexName){
	// * SysIndex : tID, colID, startPage, indexName : 2, 2, 2, 1

	char* inputChar;
	Record* r = new Record();

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",tID);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%d",colID);
	r->addValue(inputChar,2);
	delete[] inputChar;

	inputChar = new char[sizeof(long)];
	sprintf(inputChar,"%ld",startPage);
	r->addValue(inputChar,2);
	delete[] inputChar;

	r->addValue(indexName,1);

	int attrTypeSys[] = { 2, 2, 2, 1 };

	RecordSet* rs = new RecordSet(attrTypeSys,4);
	rs->addRecord(r);

	SysIndex->insertTuples(rs);

	delete rs;

	dbh->writeToPage();

	return 0;
}

void DataBaseServer::showTables(){
	RecordSet *rs;
	//Select *select = new Select(pos,1,tAttr,6);
	Select *select = new Select();
	Where *where = new Where();
	rs = SysTable->selectTuples(select,where);
	//rs = SysColumn->selectTuples();
	int n = rs->getNumOfRecords();
	int i;
	if(n>0){
		cout << n <<" tables found."<<endl;
		vector<Record*> tables = rs->getAllRecords();
		for(i=0;i<n;i++){
			cout <<endl<<tables[i]->getValues()[1];
		}
		cout <<"\n\n";
	}else
		cout<<"No tables found."<<endl;

	delete select;
	delete where;
	delete rs;
}

void DataBaseServer::showDBDetails(){
	RecordSet *rs;
	cout <<endl<< "--SysTable--"<<endl;

	Select *select = new Select();
	Where *where = new Where();

	rs = SysTable->selectTuples(select,where);

	int n = rs->getNumOfRecords();

	if(n>0){
		rs->printAll(0,rs->getNumOfRecords());
		cout << endl;
	}else
		cout<<"No tables found."<<endl;


	delete select;
	delete where;
	delete rs;

	cout << "--SysColumn--"<<endl;

	select = new Select();
	where = new Where();

	rs = SysColumn->selectTuples(select,where);

	n = rs->getNumOfRecords();
	if(n>0){
		rs->printAll(0,rs->getNumOfRecords());
		cout << endl;
	}else
		cout<<"No sysColumn entries found."<<endl;

	delete select;
	delete where;
	delete rs;

	cout << "--SysIndex--"<<endl;

	select = new Select();
	where = new Where();

	rs = SysIndex->selectTuples(select,where);

	n = rs->getNumOfRecords();
	if(n>0){
		rs->printAll(0,rs->getNumOfRecords());
		cout << endl;
	}else
		cout<<"No sysIndex entries found."<<endl;

	delete select;
	delete where;
	delete rs;

}

int DataBaseServer::setAndGetTableNum(){
	++numOfTables;
	dbh->updateTableCount();
	dbh->writeToPage();
	return numOfTables;
}

void DataBaseServer::getDataDirectory(char* dataDirLoc){
	strcpy(dataDirLoc, dataDirectory);
}

void DataBaseServer::setDataDirectory(char* dataDirLoc){
	strcpy(dataDirectory, dataDirLoc);
}

} /* namespace datamodels */
