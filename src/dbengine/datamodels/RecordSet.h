/*
 * RecordSet.h
 *
 *  Created on: Dec 28, 2012
 *      Author: latchu
 */

#ifndef RECORDSET_H_
#define RECORDSET_H_
#include "global.h"
#include "Record.h"
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

namespace datamodels {

class Record;

class RecordSet {
private:
	int *attrType;
	vector<char*> attrName;
	long numOfRecords;
	int numOfAttr;
	vector<Record*> records;
	bool colNameExist;
	int* printWidth;
public:
	RecordSet();
	RecordSet(int* attrType, int numOfAttr);
	RecordSet(vector<int> attrT, int numOfAttr);
	virtual ~RecordSet();

	int* getAttrType();
	long getNumOfRecords();
	int getNumOfAttr();
	vector<Record*> getRecords(int start,int limit);
	vector<Record*> getAllRecords();

	int addRecord(char* recStr,int recSize);//while retrieving
	int addRecord(Record* record);//while inserting

	int getSizeOf(unsigned recNo);

	void setAttrName(vector<char*> aName, int numAttr, int* pos);
	void printAll(int start, int limit);
	void printAllIndex(int start, int limit);
	void updatePrintWidth(vector<int> valSize);

	void sortResult(int attrID);

	void clearAllRecs();
};

int compare(char* valOne, char* valTwo, int valType);

} /* namespace datamodels */
#endif /* RECORDSET_H_ */
