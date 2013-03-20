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

	DataBaseServer* dbs;

public:
	Table();
	Table(int pid, bool createNew);//loading SYSINDEX(3) SYSCOLUMN(2) SYSTABLES(1)
	Table(char *tableName,vector<char*> attrName, vector<int> maxSize, vector<int> attrType, DataBaseServer* db);//creating new table
	Table(char *tableName, DataBaseServer* db);//loading old table from SYSTABLE
	virtual ~Table();

	char* getTableName();
	char** getAttributeNames();
	int* getAttributeTypes();
	int getNumOfAttr();
	DirPage* getDirPageHeader();

	int dropTable(); //TODO
	int alterTable(); //TODO

	RecordSet* selectTuples();
	RecordSet* selectTuples(Select *select, Where *where);
	int insertTuples(RecordSet* rs);
	long deleteTuples(Where *where,int* attrType, int numOfAttr);
	long updateTuples(Where *where, Modify* modify); //TODO

	int createIndex(int attrID); //TODO


};

} /* namespace datamodels */
#endif /* TABLE_H_ */
