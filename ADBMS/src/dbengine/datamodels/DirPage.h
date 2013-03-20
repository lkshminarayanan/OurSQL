/*
 * DirPage.h
 *
 *  Created on: Dec 25, 2012
 *      Author: latchu
 */

#ifndef DIRPAGE_H_
#define DIRPAGE_H_

#include "global.h"
#include "Page.h"
#include "DataPage.h"
#include "RecordSet.h"

#define MAX_DE  ((pageSize - sizeof(DirPage))/sizeof(DirEntry))

namespace dbEngine{
	class Select;
	class Where;
}

using namespace dbEngine;

namespace datamodels {

class DataPage;

class DirEntry{
private:
	long pageid;
	long tfs;
public:
	DirEntry();
	DirEntry(long,long);
	~DirEntry();

	int writeDE(char* page,long id);
	static vector<DirEntry*> getAllEntries(char *buf, long totalDEs);

	long getPageID();
	long getTFS();

	void setPageID(long pid);
	void setTFS(long tfs);
};


class DirPage:public Page {

private:
	long nextPage;
	long prevPage;
	long DEcount;//number of Directory entries
	long maxSpaceAvailable;//max Space available in any of the DE
	vector<DirEntry*> dirEntries;
	//pointer to write area
//	char* c;
public:
	DirPage();
	DirPage(long);
	virtual ~DirPage();
	DataPage* createDataPage();

	void writeToPage();

	DirPage* getNextPage();
	void setNextPage(long);

	DirPage* getPrevPage();
	void setPrevPage(long);

	long getDEcount();
	vector<DirEntry*> getDirEntries();
	long getMaxSpaceAvailable();

	void printDetails();

	int insertRecords(RecordSet* rs, int start);
	int deleteRecords(Where *where, int* attrType, int numOfAttr);
	long updateRecords(Where *where,Modify *modify, RecordSet* rs);

	int retrieveRecords(RecordSet* rs, Select* select, Where* where);

	int findNextPartialPage(int start,int recLen);

	void updateMaximumSpaceParam();
};

} /* namespace datamodels */
#endif /* DIRPAGE_H_ */
