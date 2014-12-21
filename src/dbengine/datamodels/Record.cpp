/*
 * Record.cpp
 *
 *  Created on: Dec 27, 2012
 *      Author: latchu
 */

#include "Record.h"

namespace datamodels {

Record::Record() {
	// TODO Auto-generated constructor stub
	size = 0;
}

Record::Record(int *attrType, char *recStr, int numOfAttr, int recSize) {
	//creating record from recStr and size!
	lg("DP print :"<< recStr+24);

	char *data;
	int size;
	int *offset = new int[numOfAttr+1];
	int sizeOfDir = numOfAttr*sizeof(int);
	size = 0;
	memcpy(offset,recStr,sizeOfDir);
	recSize -= sizeOfDir;
	offset[numOfAttr] = recSize;
	char *rec = recStr + sizeOfDir;

	this->size = recSize;

	lg2("Retrieving records  : ");

	//valSize.reserve(numOfAttr);

	for(int i=0;i<numOfAttr;i++){
		size = offset[i+1] - offset[i];
		valSize.push_back(size);
		lg2("value size : "<<size);
		data = new char[size + 1];	//to accomodate null char
		memcpy(data,rec+offset[i],size);
		//append null if the type is a char*
		data[size] = '\0';
		lg2(i<<" "<<data);
		values.push_back(data);
	}
	delete [] offset;
}

Record::Record(vector<int> attrType, char *recStr, int numOfAttr, int recSize){
	//creating record from recStr and size!
	lg("DP print :"<< recStr+24);

	char *data;
	int size;
	int *offset = new int[numOfAttr+1];
	int sizeOfDir = numOfAttr*sizeof(int);
	size = 0;
	memcpy(offset,recStr,sizeOfDir);
	recSize -= sizeOfDir;
	offset[numOfAttr] = recSize;
	char *rec = recStr + sizeOfDir;

	this->size = recSize;

	lg2("Retrieving records  : ");

	//valSize.reserve(numOfAttr);

	for(int i=0;i<numOfAttr;i++){
		size = offset[i+1] - offset[i];
		valSize.push_back(size);
		lg2("value size : "<<size);
		data = new char[size + 1];	//to accomodate null char
		memcpy(data,rec+offset[i],size);
		//append null if the type is a char*
		data[size] = '\0';
		lg2(i<<" "<<data);
		values.push_back(data);
	}
	delete [] offset;

}

Record::~Record() {
	int n = values.size();
	for(int i=0;i<n;i++){
		delete[] values[i];
	}

}

int Record::getSize(){
	return size + values.size()*sizeof(int);
}

vector<char*> Record::getValues(){
	return values;
}

char* Record::getValue(int colID){
	return values[colID];
}

int Record::addValueInternal(char* data,int type){
	values.push_back(data);
	int sz = strlen(data);
	valSize.push_back(sz);
	size += sz;
	return 1;
}

int Record::addValue(char* data,int type){

	//	int dataSize;
	char *data_local = new char[strlen(data)+1];

	strcpy(data_local,data);

	return addValueInternal(data_local,type);

}

int Record::addValue(int data,int type){
	char *dataC = new char[10];
	sprintf(dataC,"%d",data);

	return addValueInternal(dataC,type);

}

int Record::addValue(long data,int type){
	char *dataC = new char[10];
	sprintf(dataC,"%ld",data);

	return addValueInternal(dataC,type);

}

int Record::addValue(float data,int type){
	char *dataC = new char[10];
	sprintf(dataC,"%f",data);

	return addValueInternal(dataC,type);
}

int Record::addValue(string data,int type){
	char *dataC = new char[sizeof(data)];

	return addValueInternal(strcpy(dataC,data.c_str()),type);
}

int Record::deleteValue(unsigned attrID){
	if(attrID>=values.size()){
		return 0;
	}

	size -= strlen(values[attrID]);

	values.erase(values.begin()+attrID);
	valSize.erase(valSize.begin()+attrID);


	return 1;
}


char* Record::toRecordString(int* attrType){
	vector<char*>::size_type num = values.size();
	int *offset = new int[num];
	int offset_sz = sizeof(int)*num;
	lg2("Record Size : "<<size);
	char *data = new char[size+1];
	int sz;
	char *d;
	int loc_off = 0;
	for(vector<char*>::size_type i = 0;i < num;i++){
		d = values[i];
		lg2("value "<<i<<" : "<<d);
		sz = strlen(d);
		memcpy(data+loc_off,d,sz);
		offset[i] = loc_off;
		loc_off += sz;
	}
	lg2("data : "<<data);
	data[size] = '\0';
	char *recStr = new char[offset_sz + size + 1];
	memcpy(recStr,offset,offset_sz);
	memcpy(recStr + offset_sz,data,size+1);

	delete[] data;
	delete[] offset;
	return recStr;
}

char* Record::toPrintString(int* attrType){
	return toRecordString(attrType)+sizeof(int)*values.size();
}

int Record::modifyRecord(unsigned pos, char* data){
	if(pos > values.size()){
		error("@Record : Invalid Size in Modify Record");
		return 0;
	}
	int sizeUpdate = strlen(data);
	char* dataC = new char[sizeUpdate+1];
	strcpy(dataC,data);//local copy

	sizeUpdate -= strlen(values[pos]);
	size += sizeUpdate;

	//delete prev value;
	delete[] values[pos];
	values[pos] = dataC;
	return 1;
}

vector<int> Record::getValSize(){
	return valSize;
}

/*char* Record::toRecordString(int* attrType){
	vector<void*>::size_type num = values.size();
	int *offset = new int[num];
	int offset_sz = sizeof(int)*num;
	lg2("Record Size : "<<size);
	char *data = new char[size+1];
	int sz;
	void *d;
	int loc_off = 0;
	for(vector<void*>::size_type i = 0;i < num;i++){
		d = values[i];
		switch(attrType[i]){
		case 1://char
			lg2((char*)d);
			sz = strlen((char*)d);
			break;
		case 2://long int
			lg2(*((long*)d));
			sz = sizeof(long);
			break;
		case 3://float
			lg2(*((double*)d));
			sz = sizeof(double);
			break;
		}
		lg2("copied : "<<(char*)memcpy(data+loc_off,d,sz));
		offset[i] = loc_off;
		loc_off += sz;
		lg2("data : "<<data);
	}
	data[size+1] = '\0';
	char *recStr = new char[offset_sz + size + 1];
	memcpy(recStr,offset,offset_sz);
	lg("recstr "<<recStr);
	memcpy(recStr + offset_sz,data,size+1);
	return recStr;
}*/

} /* namespace datamodels */


