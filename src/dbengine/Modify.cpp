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
	hasMoreFlag = false;
}

Modify::Modify(int *attrType,int numOfAttr){
	numOfAttrToUpdate = 0;
	startDirEntry = 0;
	startSlotId = 1;
	limit = MAX_RETR_LIMIT;
	this->attrType = attrType;
	this->numOfAttr = numOfAttr;
	hasMoreFlag = false;
}

Modify::Modify(vector<int> attrType,int numOfAttr){
	numOfAttrToUpdate = 0;
	startDirEntry = 0;
	startSlotId = 1;
	limit = MAX_RETR_LIMIT;

	int *attrTypeArr = new int[attrType.size()];
	for(unsigned int i=0;i<attrType.size();i++){
		attrTypeArr[i] = attrType[i];
	}

	this->attrType = attrTypeArr;
	this->numOfAttr = numOfAttr;
	hasMoreFlag = false;
}

Modify::~Modify(){
	delete[] attrType;
	for(int i= 0;i<numOfAttrToUpdate;i++){
		delete[] values[i].first;
	}
}

int Modify::addUpdatedValues(char* data,int pos){
	char *dataC = new char[strlen(data)+1];
	strcpy(dataC,data);
	values.push_back(std::make_pair(dataC,pos));
	numOfAttrToUpdate++;
	return 1;
}
int Modify::addUpdatedValues(long data,int pos){
	char *dataC = new char[20];
	sprintf(dataC,"%ld",data);

	values.push_back(std::make_pair(dataC,pos));
	numOfAttrToUpdate++;
	return 1;
}
int Modify::addUpdatedValues(int data,int pos){
	char *dataC = new char[10];
	sprintf(dataC,"%d",data);

	values.push_back(std::make_pair(dataC,pos));
	numOfAttrToUpdate++;
	return 1;
}

int Modify::addUpdatedValues(float data,int pos){
	char *dataC = new char[20];
	sprintf(dataC,"%f",data);

	values.push_back(std::make_pair(dataC,pos));
	numOfAttrToUpdate++;
	return 1;
}
int Modify::addUpdatedValues(string data,int pos){
	char *dataC = new char[data.size()];

	values.push_back(std::make_pair(strcpy(dataC,data.c_str()),pos));
	numOfAttrToUpdate++;
	return 1;
}




Record* Modify::updateRecord(Record* record){
	if(numOfAttrToUpdate == 0)
		return record;
	int pos;
	char *data;
	for(int i= 0;i<numOfAttrToUpdate;i++){
		pos = values[i].second;

		data = new char[strlen(values[i].first)+1];
		strcpy(data,values[i].first);
		record->modifyRecord(pos,data);
		delete[] data;
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
