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

	void setAttrName(vector<char*> aName);
	void printAll(int start, int limit);
};

} /* namespace datamodels */
#endif /* RECORDSET_H_ */
