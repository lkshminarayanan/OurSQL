/*
 * Select.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "Select.h"

namespace dbEngine {

Select::Select() {
	numOfColumns = 0;
	limit = MAX_RETR_LIMIT;
	slotid = 1;
	datapageDirEntryid = 0;
	dirpid = 0;
}

Select::~Select() {}

Select::Select(int *pos,int numOfColumns,int *tAttr, int totalAttrCount){
	this->pos = pos;
	this->t_AttrType = tAttr;
	this->numOfColumns = numOfColumns;
	this->t_NumOfAttributes = totalAttrCount;
	attrType = new int[numOfColumns];
	for(int i = 0;i < numOfColumns; i++){
		attrType[i] = t_AttrType[pos[i]];
	}
	limit = MAX_RETR_LIMIT;
	slotid = 1;
	datapageDirEntryid = 0;
	dirpid = 0;
	moreFlag = false;
}

Record* Select::project(Record* record){
	if(numOfColumns == 0)
		return record;
	Record *newR = new Record();
	vector<char*> values = record->getValues();
	for(int i = 0;i< numOfColumns;i++){
		newR->addValue(values[pos[i]],attrType[i]);
	}
	return newR;
}

void Select::setLimit(long limit){
	this->limit = limit;
}

long Select::getStartDirPageID(){
	return dirpid;
}

int Select::getStartDirEntryID(){
	return datapageDirEntryid;
}

void Select::setStartDirPageID(long pid){
	dirpid = pid;
}

void Select::setStartDirEntryID(int sid){
	datapageDirEntryid = sid;
}

long Select::getLimit(){
	return limit;
}

void Select::setStartSlotID(int sid){
	slotid = sid;
}

int Select::getStartSlotId(){
	return slotid;
}

int* Select::getAttrType(){
	return attrType;
}

int Select::getNumOfAttr(){
	return numOfColumns;
}

int* Select::getTableAttrType(){
	return t_AttrType;
}

int Select::getTableNumOfAttr(){
	return t_NumOfAttributes;
}

bool Select::isThereMore(){
	return moreFlag;
}

void Select::thereIsMore(){
	moreFlag = true;
}

void Select::resetMoreFlag(){
	moreFlag = false;
}

void Select::logDetails(){
	cout << endl << "Select Details :";
	cout << endl << "DirPage : "<<dirpid;
	cout << endl << "DirPageSlotID : "<<datapageDirEntryid;
	cout << endl << "Slot ID : "<<slotid;
	cout << endl << "Number of Columns to retrieve : "<<numOfColumns;
	cout << endl << "LIMIT : "<<limit<<flush;
}

void Select::initializeStartParams(){
	slotid = 1;
	datapageDirEntryid = 0;
	dirpid = 0;
	moreFlag = false;
}

} /* namespace datamodels */
