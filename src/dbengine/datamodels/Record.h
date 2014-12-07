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
using namespace std;

namespace datamodels {

class Record {
private:
	vector<char*> values;
	int size;
public:
	Record();
	int addValue(char* data,int type);

	Record(int *attrType,char* recStr, int numOfAttr, int recSize);
	virtual ~Record();

	char* toRecordString(int* attrType);
	vector<char*> getValues();

	int modifyRecord(unsigned pos, char* data);

	int getSize();

};

} /* namespace datamodels */
#endif /* RECORD_H_ */
