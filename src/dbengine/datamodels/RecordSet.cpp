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

	this->attrType = attrType;
	this->numOfAttr = numOfAttr;
	numOfRecords = 0;
	colNameExist = false;
}

RecordSet::RecordSet(vector<int> attrT, int numOfAttr){
	this->numOfAttr = numOfAttr;
	numOfRecords = 0;
	int *newAttr = new int[numOfAttr];

	for(int i = 0;i<numOfAttr;i++){
		newAttr[i] = attrT[i];
	}
	this->attrType = newAttr;
	colNameExist = false;
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

	if(numOfRecords==0){
		cout << endl << "No Records Found."<<endl;
		return;
	}
	cout << endl<< "Total Records : " << numOfRecords;
	cout << endl<< "Columns : " << numOfAttr;
	cout << endl<< "Attribute Types : ";
	int i;
	for(i = 0;i<numOfAttr;i++){
		cout<<i+1<<"-";
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
		default:
			cout << "error-type-"<<attrType[i]<<"\t";
			break;
		}
	}
	cout << endl;
	if(colNameExist){
		for(i = 0;i<numOfAttr;i++){
			cout << attrName[i] <<"\t\t";
		}
	}else{
		for(i = 0;i<numOfAttr;i++){
			cout << "Col"<<i+1 <<"\t\t";
		}
	}
	cout << endl;
	int j;
	for(i = 0;i<numOfRecords;i++){
		for(j=0;j<numOfAttr;j++){
			//cout << "here here";cout.flush();
			vector<char*> val = records[i]->getValues();
			cout << val[j] << "\t\t";
		}
		cout << endl;
	}
	cout << endl;

}

int RecordSet::getSizeOf(unsigned recNo){
	if(recNo < 0||recNo > records.size())
		return 0;
	return records[recNo]->getSize();
}

void RecordSet::setAttrName(vector<char*> aName){
	attrName = aName;
	colNameExist = true;
}

} /* namespace datamodels */
