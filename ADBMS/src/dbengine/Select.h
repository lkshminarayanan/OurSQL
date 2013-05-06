/*
 * Select.h
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#ifndef SELECT_H_
#define SELECT_H_
#include "globals.h"
#include "Where.h"

namespace datamodels{
class Record;
};

using namespace datamodels;

namespace dbEngine {

class Select {
private:
	int *pos;//position of the needed columns.
	int *attrType;//type of needed columns.
	int numOfColumns;

	//number of records to be retrieved.
	bool moreFlag;
	long limit;//0 - no limits. otherwise yes.
	long dirpid;//dirpageid to start retrieving.
	int datapageDirEntryid;//datapage - dataentry id to start.
	int slotid;//slot id to start.

	//table data
	int *t_AttrType;
	int t_NumOfAttributes;

public:
	Select();
	Select(int *pos,int numOfSelectAttributes,int *attrType, int totalAttributes);
	Select(vector<int> pos,int numOfSelectAttributes,vector<int> attrType, int totalAttributes);
	virtual ~Select();
	Record* project(Record* actualRecord);

	long getStartDirPageID();
	int getStartDirEntryID();
	void setStartDirPageID(long);
	void setStartDirEntryID(int);
	long getLimit();
	void setLimit(long);
	int getStartSlotId();
	void setStartSlotID(int sid);

	int* getAttrType();
	int getNumOfAttr();

	int* getTableAttrType();
	int getTableNumOfAttr();

	bool isThereMore();
	void thereIsMore();
	void resetMoreFlag();

	void logDetails();
	void initializeStartParams();

};

} /* namespace datamodels */
#endif /* SELECT_H_ */
