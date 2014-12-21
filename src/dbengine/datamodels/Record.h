/*
 * Record.h
 *
 *  Created on: Dec 27, 2012
 *      Author: latchu
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <vector>
#include <string.h>
#include "global.h"
#include "../globals.h"
using namespace std;

namespace datamodels {

class Record {
private:
	vector<char*> values;
	int size;
	vector<int> valSize;


	int addValueInternal(char* data,int type);

public:
	Record();
	int addValue(char* data,int type);
	int addValue(int data,int type);
	int addValue(long data,int type);
	int addValue(float data,int type);
	int addValue(string data,int type);

	int deleteValue(unsigned attrID);

	Record(int *attrType,char* recStr, int numOfAttr, int recSize);
	Record(vector<int> attrType,char* recStr, int numOfAttr, int recSize);
	virtual ~Record();

	char* toRecordString(int* attrType);
	char* toPrintString(int* attrType);
	vector<char*> getValues();
	char* getValue(int colID);

	int modifyRecord(unsigned pos, char* data);

	int getSize();

	vector<int> getValSize();

};

} /* namespace datamodels */
#endif /* RECORD_H_ */
