/*
 * Table.h
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#ifndef TABLE_H_
#define TABLE_H_

#include "globals.h"
#include "datamodels/DirPage.h"
#include  "Select.h"
#include "Where.h"
#include "Modify.h"
#include "DataBaseServer.h"
#include "Modify.h"
#include <time.h>
#include <ctime>
#include <algorithm>

using namespace datamodels;

namespace dbEngine {

class DataBaseServer;

class Table {

private :
	char *tName;
	vector<char*> attrName;
	vector<int> attrType;
	vector<int> maxColSize;
	DirPage *tHeader;
	int numOfAttrColumns;
	long tID;
	long numOfRows;

	DataBaseServer* dbs;

public:
	Table();
	Table(int pid, bool createNew);//loading SYSINDEX(3) SYSCOLUMN(2) SYSTABLES(1)
	Table(char *tableName,vector<char*> attrName, vector<int> maxSize, vector<int> attrType, DataBaseServer* db);//creating new table
	Table(char *tableName, DataBaseServer* db);//loading old table from SYSTABLE
	virtual ~Table();

	char* getTableName();
	vector<char*> getAttributeNames();
	vector<int> getAttributeTypes();
	int getNumOfAttr();
	DirPage* getDirPageHeader();
	vector<string> getAttributeNamesString();

	int dropTable(); //TODO
	int alterTable(); //TODO

	RecordSet* selectTuples();
	RecordSet* selectTuples(Select *select, Where *where);
	RecordSet* selectTuples(int type,vector<char*> selectColumNames);//prepares select and where and calls the above select internally

	int insertTuples(RecordSet* rs);
	int insertTuples(vector<char*> aName,vector<char*> value);
	int insertTuples(char* tName,RecordSet* rs);//insert directly with table name.

	long deleteTuples(Where *where,int* attrType, int numOfAttr);
	long updateTuples(Where *where, Modify* modify); //TODO

	int createIndex(int attrID); //TODO

	long getNumOfRows();

};

} /* namespace datamodels */
#endif /* TABLE_H_ */
