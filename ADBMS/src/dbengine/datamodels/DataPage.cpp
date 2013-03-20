/*
 * DataPage.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: latchu
 */

#include "DataPage.h"

namespace datamodels {

DataPage::DataPage(){
	//TODO: New Page. initialize dp obj and write it to page.
	totalRecords = totalSlots = 0;//no. of slots in the page
	totalFreeSize = pageSize - sizeof(DataPage);//total size
	contigFreeSize = totalFreeSize;// the contiguous free space
	contigFreeSpaceStart = 0;//offset of the start of contiguous free space from 'c'
	//The Storage Area
	c = p + sizeof(DataPage);
	memcpy(p,this,sizeof(DataPage));
	writePage();
}

DataPage::DataPage(long pid):Page(pid){
	DataPage* dp;
	char* buf = new char[sizeof(DataPage)];
	memcpy(buf,p,sizeof(DataPage));
	dp = (DataPage*)buf;
	totalSlots = dp->totalSlots;
	totalRecords = dp->totalRecords;
	totalFreeSize = dp->totalFreeSize;
	contigFreeSize = dp->contigFreeSize;
	contigFreeSpaceStart = dp->contigFreeSpaceStart;
	c = p + sizeof(DataPage);
	printDetails();
}



DataPage::~DataPage() {
	// TODO Auto-generated destructor stub
	writeToPage();
	delete [] p;
}


long DataPage::getNextPage() const {
	return nextPage;
}

void DataPage::setNextPage(long nextPage) {
	this->nextPage = nextPage;
}

char* DataPage::getC(){
	return c;
}

long DataPage::getContigFreeSize() const {
	return contigFreeSize;
}

void DataPage::setContigFreeSize(long contigFreeSize) {
	this->contigFreeSize = contigFreeSize;
}

int DataPage::getContigFreeSpaceStart() const {
	return contigFreeSpaceStart;
}

void DataPage::setContigFreeSpaceStart(int contigFreeSpaceStart) {
	this->contigFreeSpaceStart = contigFreeSpaceStart;
}

long DataPage::getPrevPage() const {
	return prevPage;
}

void DataPage::setPrevPage(long prevPage) {
	this->prevPage = prevPage;
}

long DataPage::getTotalFreeSize() const {
	return totalFreeSize;
}

void DataPage::setTotalFreeSize(long totalFreeSize) {
	this->totalFreeSize = totalFreeSize;
}

int DataPage::getTotalSlots() const {
	return totalSlots;
}

void DataPage::setTotalSlots(int totalSlots) {
	this->totalSlots = totalSlots;
}

int DataPage::getTotalRecords() const {
	return totalRecords;
}

void DataPage::setTotalRecords(int totalRecords) {
	this->totalRecords = totalRecords;
}

//void DataPage::setP(Page* p) {
//	this->p = p;
//}


int DataPage::insertRecord(char* record, int size ){
	//get DB for the page;
	// create dbpage and call :	DataPage dp = new DataPage(pid);dp.insertRecord;
	int recLength = size;
	int slotD_sz = sizeof(SlotDir);
	int tSize;
	int slotNumber = 0;
	vector<SlotDirPtr> sdp;

	if(totalSlots > totalRecords){
		//possible slotID availability
		tSize = recLength;

		lg2("@DataPage "<<pageid<<"  : Inserting Record in dataPage "<<pageid<<". Space Req. : "<<tSize );

		if(totalFreeSize<tSize){//no sufficient space
			error("@DataPage "<<pageid<<"  : No space for inserting. "<<tSize - totalFreeSize<<" more space req.");
			return -1;
		}

		//always iterate between all slots and check for empty slot
		sdp = SlotDir::getAllSlots(p,totalSlots);
		int bf_diff, bf_diff_temp;//for bestfit
		bf_diff = pageSize + 1;//for maximum
		for(int i=1;i<=totalSlots;i++){
			//whatever happens go until we find a perfect fit!.we need the besttt fittu!.
			int size = sdp[i]->size;
			if(size < 0){
				size *= -1;
				if(recLength == size){
					//Bingo!perfect fit!.We can break here!!
					lg2("@DataPage "<<pageid<<"  : Found a perfect fit.");
					slotNumber = i;
					break;

				}else if(recLength < size){
					//possible candidate

					bf_diff_temp = size - recLength;
					if(bf_diff_temp < bf_diff){
						bf_diff = bf_diff_temp;
						slotNumber = i;
					}
				}
			}
			//ignore empty slot ids here. Take care of that later.
		}
		if(slotNumber !=0){
			//A winner with already allocated space!
			lg2("@DataPage "<<pageid<<"  : Reusing Slot - "<<slotNumber);
			SlotDirPtr sd = sdp[slotNumber];
			sd->size = recLength;
			memcpy(c+sd->offset,record,recLength);
			sd->writeSlot(p,slotNumber);

			goto UPDATE_PARAMS;

		}else{
			//check if there is contiguous free size to allocate record.
			if(contigFreeSize < tSize){
				//if cfs is less than req size, defragment the page.
				defragmentPage();
			}
			slotNumber = SlotDir::getEmptySlotID(p,totalSlots);
			if(slotNumber == 0){//no empty slotID!.
				//update tSize
				tSize += SLOT_SZ;
				lg2("@DataPage "<<pageid<<"  : Updated space requirement : "<<tSize);

				//now have to check if insertion is possible with updated tSize
				if(tSize<=totalFreeSize){//insertion possible
					if(tSize>contigFreeSize){
						defragmentPage();//happens very rarely. guaranteed called atmost one every insertion.
						//now insertion is possible
					}
				}else{
					error("@DataPage "<<pageid<<"  : No space for inserting. "<<tSize - totalFreeSize<<" more space req.");
					return -1;
				}

				//update the slotNumber
				slotNumber = ++totalSlots;
				lg2("@DataPage "<<pageid<<"  : Creating Slot-ID - "<<slotNumber);
			}else
				lg2("@DataPage "<<pageid<<"  : Reusing Slot-ID - "<<slotNumber);

			goto START_COPY;
		}
	}else{
		tSize = slotD_sz + recLength;

		lg2("@DataPage "<<pageid<<"  : Inserting Record in dataPage "<<pageid<<". Space Req. : "<<tSize );

		if(totalFreeSize<tSize){//no sufficient space
			error("@DataPage "<<pageid<<"  : No space for inserting. "<<tSize - totalFreeSize<<" more space req.");
			return -1;
		}

		if(contigFreeSize < tSize){
			//if cfs is less than req size, defragment the page.
			defragmentPage();
		}

		slotNumber = ++totalSlots;
		lg2("@DataPage "<<pageid<<"  : Creating Slot-ID - "<<slotNumber);
	}

	START_COPY:{
		memcpy(c+contigFreeSpaceStart,record,recLength);
		SlotDirPtr slot = new SlotDir(contigFreeSpaceStart,recLength);
		slot->writeSlot(p,slotNumber);
		contigFreeSpaceStart += recLength;
		contigFreeSize -= tSize;
	}

	UPDATE_PARAMS:
	totalFreeSize -= tSize;
	totalRecords++;
	writeToPage();
	return slotNumber;//subam
}

int DataPage::retrieveRecord(int sid, char *&record){
	//SLOT ID starts from 1.
	if(sid < 1 || sid > totalSlots){//invalid slotid
		error("Invalid slot id");
		record = NULL;
		return 0;

	}
	lg2("Accessing slot "<<sid);
	SlotDirPtr sdp = new SlotDir(p,sid);
	if(sdp->size <= 0){//deleted record
		error("Invalid slot id (deleted id)");
		record = NULL;
		return 0;
	}
	record = new char[sdp->size+1];

	memcpy(record,c+sdp->offset,sdp->size);
	record[sdp->size] = '\0';
	return sdp->size;//Subam
}

int DataPage::deleteRecord(int sid){
	if(sid < 1 || sid > totalSlots){//invalid slotid
		error("Invalid slot id");
		return 0;
	}
	SlotDirPtr sdp = new SlotDir(p,sid);
	if(sdp->size <= 0){//already deleted
		lg("Slot "<<sid<<" already deleted");
		return 1;
	}
	totalFreeSize += sdp->size;
	totalRecords--;
	sdp->size *= -1;
	sdp->writeSlot(p,sid);
	lg("Record at slot "<<sid<<" deleted");
	writeToPage();
	return 1;
}

int DataPage::defragmentPage(){
	lg("Defragmenting page "<<pageid);
	if(totalFreeSize == contigFreeSize)
		lg("Defrag not required.");
	vector<SlotDirPtr> sdp = SlotDir::getAllSlots(p,totalSlots);
	int i;
	SlotDirPtr sd;
	flag lastEmptySlot = 0;//to find out the last consecutive empty slots and delete them.
	char *locHead = new char[pageSize-sizeof(DataPage)];
	char *loc = locHead;
	int totalLen = 0;
	for(i=1;i<=totalSlots;i++){
		sd = sdp[i];
		if(sd->size > 0){
			//			lg2("Rewriting rec "<<i);
			memcpy(loc,c+sd->offset,sd->size);
			sd->offset = loc - locHead;
			loc += sd->size;
			totalLen += sd->size;
			lastEmptySlot = 0;
		}else{
			//			lg2("Ignoring rec "<<i);
			sd->offset = 0;
			sd->size = 0;
			if(lastEmptySlot == 0)
				lastEmptySlot = i;
		}
		sd->writeSlot(p,i);
	}
	if(lastEmptySlot != 0){
		totalFreeSize += ((totalSlots - lastEmptySlot + 1)*sizeof(SlotDir));
		totalSlots = lastEmptySlot - 1;
	}
	contigFreeSpaceStart = loc - locHead;
	contigFreeSize = totalFreeSize;
	memcpy(c,locHead,totalLen);
	writeToPage();
	return 1;
}

void DataPage::printDetails(){
	lg2("DataPage");
	lg2("Page : "<< pageid);
	lg2("TFS : " << totalFreeSize);
	lg2("CFS : " << contigFreeSize);
	lg2("Total slots : "<<totalSlots);
	lg2("Records : "<<totalRecords);
	lg2("");
}

int DataPage::retrieveRecords(RecordSet *rs,Select* select,Where* where){
	if(totalRecords == 0){
		lg2("@DataPage_"<<pageid<<" : No Records to retrieve");
		return 0;
	}
	int recSize;
	char *recStr;
	Record *record;
	int noOfRecs = 0;
	int *attrType_table = rs->getAttrType();
	int numOfAttr_table = rs->getNumOfAttr();
	for(int i=select->getStartSlotId();i<=totalSlots;i++){
		recSize = retrieveRecord(i,recStr);
		if (recSize!=0){
			record = new Record(attrType_table,recStr,numOfAttr_table,recSize);
			//TODO : check for valid records here!
			if(where->testRecord(record)){
				lg2("Adding Record "<<i<<" from Page "<<pageid);
				rs->addRecord(select->project(record));
				noOfRecs ++;
			}
		}
		if(select->getLimit()!=0&&noOfRecs >= select->getLimit()){
			select->setStartSlotID(i+1);
			select->thereIsMore();
			return noOfRecs;
		}
	}
	if(select->getLimit()!=0)
		select->setLimit(select->getLimit()-noOfRecs);
	if(select->isThereMore()){
		//reset select's hasMore Parameters
		select->initializeStartParams();
	}

	return noOfRecs;
}

int DataPage::deleteRecords(Where *where,int* attrType, int numOfAttr){
	if(totalRecords == 0){
		lg2("@DataPage_"<<pageid<<" : No Records to delete");
		return 0;
	}
	int noOfRecs = 0;
	int recSize;
	char *recStr;
	Record* record;
	for(int i=1;i<=totalSlots;i++){
		recSize = retrieveRecord(i,recStr);
		if (recSize!=0){
			record = new Record(attrType,recStr,numOfAttr,recSize);
			//TODO : check for valid records here!
			if(where->testRecord(record)){
				lg2("Deleting Record "<<i<<" from Page "<<pageid);
				deleteRecord(i);
				noOfRecs ++;
			}
		}
	}
	writeToPage();
	return noOfRecs;

}

int DataPage::updateRecords(Where *where,Modify *modify, RecordSet* rs){
	if(totalRecords == 0){
		lg2("@DataPage_"<<pageid<<" : No Records to delete");
		return 0;
	}
	int noOfRecs = 0;
	int recSize;
	char *recStr;
	Record* record;
	for(int i=modify->getStartSlotID();i<=totalSlots;i++){
		recSize = retrieveRecord(i,recStr);
		if (recSize!=0){
			record = new Record(modify->getAttrType(),recStr,modify->getNumOfAttr(),recSize);
			//TODO : check for valid records here!
			if(where->testRecord(record)){
				lg2("Updating and deleting Record "<<i<<" from Page "<<pageid);
				rs->addRecord(modify->updateRecord(record));
				deleteRecord(i);
				noOfRecs ++;
			}
		}
		if(noOfRecs == modify->getLimit()){
			writeToPage();
			modify->thereIsMore();
			modify->setStartSlotID(i+1);
			return noOfRecs;
		}
	}
	writeToPage();

	modify->resetMoreFlag();

	return noOfRecs;
}

void DataPage::writeToPage(){
	memcpy(p,this,sizeof(DataPage));
	writePage();
}




/*
 * Slot Directory definitions
 *
 */
SlotDir::SlotDir(){}
SlotDir::SlotDir(long oset,int sz){
	offset = oset;
	size = sz;
}
SlotDir::SlotDir(char* buf, int sid){
	char* slotLoc = buf + pageSize - sid*SLOT_SZ;
	char* slot = new char[ sizeof(SlotDir) ] ;
	memcpy(slot,slotLoc,sizeof(SlotDir));
	SlotDirPtr sdp = (SlotDirPtr)slot;
	offset = sdp->offset;
	size = sdp->size;
}
int SlotDir::writeSlot(char* buf, int sid){
	char* slot_entry = buf + pageSize - (sid)*SLOT_SZ;
	memcpy(slot_entry,this,sizeof(SlotDir));
	return 1;
}
vector<SlotDirPtr> SlotDir::getAllSlots(char* buf, int totalSlots){
	char* slotLoc = buf + pageSize - SLOT_SZ;//slot no. 1!
	int i = 1;
	char* slot;
	SlotDirPtr sdp;
	vector<SlotDirPtr> sDir;
	sDir.push_back(new SlotDir());//dummy slot to make 0 - totalslots-1 to 1 - totalSlots
	for (i=1;i<=totalSlots;i++,slotLoc = slotLoc - sizeof(SlotDir)){
		slot = new char[ sizeof(SlotDir) ] ;
		memcpy(slot,slotLoc,sizeof(SlotDir));
		sdp = (SlotDirPtr)slot;
		sDir.push_back(sdp);
	}
	return sDir;
}

void SlotDir::printAllSlots(char* buf, int totalSlots){
	char* slotLoc = buf + pageSize - SLOT_SZ;//slot no. 1!
	int i = 1;
	char* slot = new char[ sizeof(SlotDir) ] ;;
	SlotDirPtr sdp;
	for (i=1;i<=totalSlots;i++,slotLoc = slotLoc - sizeof(SlotDir)){
		memcpy(slot,slotLoc,sizeof(SlotDir));
		sdp = (SlotDirPtr)slot;
		cout << endl<< "Slot "<<i<<" - Offset "<<sdp->offset<<" - Size "<<sdp->size;
	}
}

int SlotDir::getEmptySlotID(char* buf,int totalSlots){
	char* slotLoc = buf + pageSize - SLOT_SZ;//slot no. 1!
	int i = 1;
	char* slot = new char[ sizeof(SlotDir) ] ;;
	SlotDirPtr sdp;
	for (i=1;i<=totalSlots;i++,slotLoc = slotLoc - sizeof(SlotDir)){
		memcpy(slot,slotLoc,sizeof(SlotDir));
		sdp = (SlotDirPtr)slot;
		if(sdp->size == 0)
			return i;
	}
	return 0;
}


} /* namespace datamodels */

//using namespace datamodels;
//int main(){
//
//
//
//	DataPage *dp = new DataPage(1);
//
//	DataPage *dp = new DataPage();
//
//	char elements[][20] = {
//			"January",
//			"February",
//			"March",//3
//			"April",
//			"May",
//			"June",//6
//			"July",
//			"August",
//			"September",//9
//			"October",
//			"November",
//			"December",//12
//			"Sunday",
//			"Monday",
//			"Tuesday",//15
//			"Wednesday",
//			"Thursday",
//			"Friday",//18
//			"Saturday"
//	};
//
//	for(int i=0;i<140;i++){
//		dp->insertRecord(elements[i%19]);
//	}
//
//
//	dp->deleteRecord(135);
//	dp->deleteRecord(136);
//	dp->deleteRecord(16);
//	dp->deleteRecord(121);
//	dp->deleteRecord(89);
//	dp->deleteRecord(101);
//
//	char test[] = "1234";
//	dp->insertRecord(test);
//	dp->printDetails();
//
//	dp->defragmentPage();
//	dp->printDetails();
//
//	dp->printAllRecords();
//
//	SlotDir::printAllSlots(dp->getPage(),dp->getTotalSlots());
//
//	return 0;
//
//}
