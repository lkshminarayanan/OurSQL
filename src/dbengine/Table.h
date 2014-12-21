/*
 * Table.h
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#ifndef TABLE_H_
#define TABLE_H_

extern bool indexStatus;

#include "globals.h"
#include "datamodels/DirPage.h"
#include  "Select.h"
#include "Where.h"
#include "Modify.h"
#include "Alter.h"
#include "DataBaseServer.h"
#include "Modify.h"
#include <time.h>
#include <ctime>
#include <algorithm>

#define MAX_COLSIZE 50

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

	int opStatus;//operation being performed curr.ly.

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

	RecordSet* selectTuples();
	RecordSet* selectTuples(Select *select, Where *where);
	RecordSet* selectTuples(int type,vector<char*> selectColumNames);//prepares select and where and calls the above select internally

	int insertTuples(RecordSet* rs);
	int insertTuples(vector<char*> aName,vector<char*> value);
	int insertTuples(char* tName,RecordSet* rs);//insert directly with table name.

	long deleteTuples(Where *where,int* attrType, int numOfAttr);
	long deleteTuples(Where *where,vector<int> attrType, int numOfAttr);
	long deleteTuples(Where *where);

	int alterTable(string tblName);//rename
	int alterTable(string oldName, string colName);//column Name rename
	int alterTableDropColumn(string colName);//drop column
	int alterTable(vector<string> colName,vector<int> type,vector<int> maxSize);//add-drop column

	long updateTuples(Where *where, Modify* modify);

	long getNumOfRows();

	int getColumnPos(string colName);

	char* getMemCharfromInt(char *val);
	char* getMemCharfromDouble(char* val);
	char* getMemCharfromChar(char* val);

	int deleteTable();

	void logDetails();


};

} /* namespace datamodels */
#endif /* TABLE_H_ */
