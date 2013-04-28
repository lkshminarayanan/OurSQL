/*
 * Modify.cpp
 *
 *  Created on: Jan 9, 2013
 *      Author: latchu
 */

#include "Modify.h"

namespace dbEngine {

Modify::Modify() {
	numOfAttrToUpdate = 0;
	startDirEntry = 0;
	startSlotId = 1;
	limit = MAX_RETR_LIMIT;
}

Modify::Modify(int *attrType,int numOfAttr){
	numOfAttrToUpdate = 0;
	startDirEntry = 0;
	startSlotId = 1;
	limit = MAX_RETR_LIMIT;
	this->attrType = attrType;
	this->numOfAttr = numOfAttr;
}

int Modify::addUpdatedValues(char* data,int pos){
	values.push_back(std::make_pair(data,pos));
	numOfAttrToUpdate++;
	return 1;
}

Record* Modify::updateRecord(Record* record){
	if(numOfAttrToUpdate == 0)
		return record;
	int pos;
	for(int i= 0;i<numOfAttrToUpdate;i++){
		pos = values[i].second;
		record->modifyRecord(pos,values[i].first);
	}
	lg("updating rec");
	return record;
}

bool Modify::isThereMore(){
	return hasMoreFlag;
}

void Modify::thereIsMore(){
	hasMoreFlag = true;
}

void Modify::resetMoreFlag(){
	hasMoreFlag = false;
	startDirEntry = 0;
	startSlotId = 1;
}

long Modify::getStartDirEntry(){
	return startDirEntry;
}

long Modify::getStartSlotID(){
	return startSlotId;
}

void Modify::setStartDirEntry(long de_ID){
	startDirEntry = de_ID;
}

void Modify::setStartSlotID(long slotid){
	startSlotId = slotid;
}

void Modify::setLimits(long l){
	limit = l;
}

void Modify::resetLimits(){
	limit = MAX_RETR_LIMIT;
}

long Modify::getLimit(){
	return limit;
}

int* Modify::getAttrType(){
	return attrType;
}

int Modify::getNumOfAttr(){
	return numOfAttr;
}

} /* namespace datamodels */
