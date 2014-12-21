/*
 * DataBaseServer.h
 *
 *  Created on: Jan 6, 2013
 *      Author: latchu
 */

#ifndef DATABASESERVER_H_
#define DATABASESERVER_H_

#include "Table.h"
#include "globals.h"
#include "datamodels/DataBaseHeader.h"

namespace dbEngine {

class Table;

class DataBaseServer {
private:
	static char dataDirectory[FILENAME_MAX];

public:
	int valid;
	DataBaseHeader *dbh;
	Table *SysTable;
	Table *SysColumn;
	Table *SysIndex;

	char* dbName;

	int sysAttrType[3][6];

	int numOfTables;

	int isValid();

	/*
	 * SysTable : tID, tName, noOfAttr, recs, startDirPage, maxColSize : 1, 2, 2, 2, 2
	 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
	 * SysIndex : tID, colID, startPage : 2, 2, 2, 1
	 */


	DataBaseServer();
	DataBaseServer(char* DataBaseName, bool creatNew);
	virtual ~DataBaseServer();
	void showTables();
	void showDBDetails();
	int dropDataBase(char* DataBaseName);
	DirPage* getDirPage(char* tName);
	int setAndGetTableNum();

	int addToSysTable(char* tName, int noOfAttr, long startDirPage);
	int addToSysColumn(long tID, char* colName, int colID, int maxSize, int type, int isIndexed);
	int addToSysIndex(long tID, int colID, long startPage, char* indexName);

	static void getDataDirectory(char *dataDirLoc);
	static void setDataDirectory(char *dataDirLoc);
};

} /* namespace datamodels */
#endif /* DATABASESERVER_H_ */
