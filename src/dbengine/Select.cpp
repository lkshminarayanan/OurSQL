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
	moreFlag = false;
	indexMode = false;
	pos = NULL;
	t_AttrType = NULL;
	attrType = NULL;
}

Select::~Select() {
	delete[] pos;
	delete[] t_AttrType;
	delete[] attrType;
}

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
	indexMode = false;
}

Select::Select(vector<int> pos,int numOfColumns,vector<int> tAttr, int totalAttrCount){
	this->pos = new int[numOfColumns];
	memcpy(this->pos,pos.data(),sizeof(int)*numOfColumns);
	this->t_AttrType = new int[totalAttrCount];
	memcpy(this->t_AttrType,tAttr.data(),sizeof(int)*totalAttrCount);
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
	indexMode = false;
}

void Select::updateParamsForSelectStar(int numOfColumns,vector<int> tAttr, int totalAttrCount){
	int n,i;

	n = tAttr.size();
	this->t_AttrType = new int[n];
	for(i=0;i<n;i++){
		this->t_AttrType[i] = tAttr[i];
	}

	this->numOfColumns = numOfColumns;
	this->t_NumOfAttributes = totalAttrCount;
	attrType = new int[numOfColumns];
	for(int i = 0;i < numOfColumns; i++){
		attrType[i] = t_AttrType[pos[i]];
	}
}

Record* Select::project(Record* record, long pageID, int slotID){
	if(numOfColumns == 0)
		return record;
	Record *newR = new Record();
	vector<char*> values = record->getValues();

	for(int i = 0;i< numOfColumns;i++){
		newR->addValue(values[pos[i]],attrType[i]);
	}

	if(indexMode){
		//Works only for single column index
		newR->addValue(pageID,2);
		newR->addValue(slotID,2);
	}

	delete record;
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

int* Select::getProjectPos(){
	return pos;
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

void Select::setIndexModeOff(){
	indexMode = false;
}

void Select::setIndexModeOn(){
	indexMode = true;
}

} /* namespace datamodels */
