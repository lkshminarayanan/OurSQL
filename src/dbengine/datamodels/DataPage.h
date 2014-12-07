/*
 * DataPage.h
 *
 *  Created on: Dec 13, 2012
 *      Author: latchu
 */

#ifndef DATAPAGE_H_
#define DATAPAGE_H_

#include "../Select.h"
#include "Page.h"
#include "string.h"
#include "RecordSet.h"

#define SLOT_SZ sizeof(SlotDir);
#define EMPTY_SLOT (pageSize + 1);

namespace dbEngine{
	class Select;
	class Where;
}

using namespace dbEngine;

namespace datamodels {

class DataPage: public Page {
private:
	long nextPage;
	long prevPage;
	int totalSlots;//no. of slots in the page
	int totalRecords;//no. of valid records in the page
	long totalFreeSize;// total free space in the page
	long contigFreeSize;// the contiguous free space
	int contigFreeSpaceStart;//offset of the start of contiguous free space
	//The Storage Area
	char *c;
	//row offset here

public:
	DataPage();
	DataPage(long);
	virtual ~DataPage();

	long getNextPage() const;
	void setNextPage(long);

	long getPrevPage() const;
	void setPrevPage(long);

	char* getC();

	long getContigFreeSize() const;
	void setContigFreeSize(long);

	int getContigFreeSpaceStart() const;
	void setContigFreeSpaceStart(int);

	long getTotalFreeSize() const;
	void setTotalFreeSize(long);

	int getTotalSlots() const;
	void setTotalSlots(int);

	int getTotalRecords() const;
	void setTotalRecords(int);

	void writeToPage();

	void printDetails();

	int insertRecord(char* recStr, int size);
//	char* retrieveRecord(int slotID);
	int retrieveRecord(int slotID, char *&record);
	int deleteRecord(int slotID);

	int defragmentPage();

	int retrieveRecords(RecordSet *rs,Select *select,Where *where);
	//change return type. pass constraint as input.
	//also pass limit to the page to get particular inputs
	int deleteRecords(Where *where, int* attrType, int numOfAttr);
	int updateRecords(Where *where,Modify *modify, RecordSet* rs);



};

class SlotDir{
public:
	long offset;
	int size;

	SlotDir();
	SlotDir(long,int);//new slot
	SlotDir(char*,int);//load slot
	int writeSlot(char*,int);

	static vector<SlotDir*> getAllSlots(char*,int);
	static void printAllSlots(char*, int);
	static int getEmptySlotID(char*,int);
};

typedef SlotDir* SlotDirPtr ;


} /* namespace datamodels */
#endif /* DATAPAGE_H_ */
