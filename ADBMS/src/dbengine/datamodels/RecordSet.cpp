/*
 * RecordSet.cpp
 *
 *  Created on: Dec 28, 2012
 *      Author: latchu
 */

#include "RecordSet.h"

namespace datamodels {

RecordSet::RecordSet() {}

RecordSet::RecordSet(int *attrType,int numOfAttr) {
	// TODO Auto-generated constructor stub
	this->attrType = attrType;
	this->numOfAttr = numOfAttr;
	numOfRecords = 0;
}

RecordSet::RecordSet(vector<int> attrT, int numOfAttr){
	this->numOfAttr = numOfAttr;
	numOfRecords = 0;
	int *newAttr = new int[numOfAttr];

	for(int i = 0;i<numOfAttr;i++){
		newAttr[i] = attrT[i];
	}
	this->attrType = newAttr;
}

RecordSet::~RecordSet() {
	for(vector<Record*>::size_type i =0;i<records.size();i++){
		delete records[i];
	}
}

int* RecordSet::getAttrType(){
	return attrType;
}

int RecordSet::getNumOfAttr(){
	return numOfAttr;
}

long RecordSet::getNumOfRecords(){
	return numOfRecords;
}

vector<Record*> RecordSet::getAllRecords(){
	return records;
}

int RecordSet::addRecord(char* recStr,int recSize){
	Record *rec = new Record(attrType,recStr,numOfAttr,recSize);
	records.push_back(rec);
	numOfRecords++;
	return 1;
}

int RecordSet::addRecord(Record* record){
	records.push_back(record);
	numOfRecords++;
	return 1;
}

vector<Record*> RecordSet::getRecords(int start,int limit){
	vector<Record*> recs;
	vector<Record*>::size_type end = start + limit;
	Record *rec;
	for(vector<Record*>::size_type i = start; i < end; i++){
		rec = records[i];
		recs.push_back(rec);
	}
	return recs;
}

void RecordSet::printAll(int start, int limit){
	cout << endl<< "Total Records : " << numOfRecords;
	cout << endl<< "Columns : " << numOfAttr;
	cout << endl<< "Attribute Types : ";
	int i;
	for(i = 0;i<numOfAttr;i++){
		switch(attrType[i]){
		case 1:
			cout << "varchar\t";
			break;
		case 2:
			cout << "int\t";
			break;
		case 3:
			cout << "real\t";
			break;
		}
	}
	cout << endl;
	for(i = 0;i<numOfAttr;i++){
		cout << "Column "/*attrName[i]*/ << i+1 <<"\t\t";
	}
	cout << endl;
	int j;
	int end = start + limit;
	for(i = 0;i<numOfRecords;i++){
		for(j=0;j<numOfAttr;j++){
			vector<char*> val = records[i]->getValues();
			cout << val[j] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;

}

int RecordSet::getSizeOf(int recNo){
	if(recNo < 0||recNo > records.size())
		return 0;
	return records[recNo]->getSize();
}



} /* namespace datamodels */
