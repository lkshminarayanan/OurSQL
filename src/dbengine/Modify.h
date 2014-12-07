/*
 * Modify.h
 *
 *  Created on: Jan 9, 2013
 *      Author: latchu
 */

#ifndef MODIFY_H_
#define MODIFY_H_

#include "globals.h"

namespace dbEngine {

class Modify {
private:
	int numOfAttrToUpdate;
	vector<pair<char*, int> > values;//position and values.

	//table data
	int *attrType;
	int numOfAttr;

	//hasMore?
	bool hasMoreFlag;
	long startDirEntry;
	long startSlotId;
	long limit;
public:
	Modify();
	Modify(int* attrType, int numOfAttr);
	Modify(vector<int> attrType, int numOfAttr);
	int addUpdatedValues(char* data, int pos);

	Record* updateRecord(Record* record);

	bool isThereMore();
	void thereIsMore();
	void resetMoreFlag();

	long getStartDirEntry();
	long getStartSlotID();
	void setStartDirEntry(long de_ID);
	void setStartSlotID(long slotid);

	void setLimits(long l);
	long getLimit();
	void resetLimits();

	int* getAttrType();
	int getNumOfAttr();
};


} /* namespace datamodels */
#endif /* MODIFY_H_ */
