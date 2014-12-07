/*
 * RecordID.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "RecordID.h"

namespace dbEngine {
char RecordID::keyTypeStr[3][7] = {"string","int","float"};

RecordID::RecordID(long pid, long slotid, char* keyValue, int kType){//during insertion
	this->pageId = pid;
	this->slotId = slotid;
	this->keyValue = keyValue;
	this->keyType = kType;
}

RecordID::RecordID(long pid, long slotid){//during retrieval from the index
	this->pageId = pid;
	this->slotId = slotid;
}

long RecordID::getPageNo(){
	return pageId;
}

long RecordID::getSlotID(){
	return slotId;
}

char* RecordID::getKeyValue(){
	return keyValue;
}

char* RecordID::getKeyType(){
	return keyTypeStr[keyType+1];
}

void RecordID::setPageNo(long pid){
	this->pageId = pid;
}

void RecordID::setSlotID(long slotid){
	this->slotId = slotid;

}

void RecordID::setKeyValue(char* key){
	this->keyValue = key;
}

void RecordID::setKeyType(int kType){
	this->keyType = kType;
}

}
