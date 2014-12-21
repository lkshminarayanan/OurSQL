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

	this->attrType = new int[numOfAttr];
	for(int i=0;i<numOfAttr;i++){
		this->attrType[i] = attrType[i];
	}
	this->numOfAttr = numOfAttr;
	numOfRecords = 0;
	colNameExist = false;
	printWidth = new int[numOfAttr];
	for(int i = 0;i<numOfAttr;i++){
		printWidth[i] = 0;
	}

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
	printWidth = new int[numOfAttr];
	for(int i = 0;i<numOfAttr;i++){
		printWidth[i] = 0;
	}
}

RecordSet::~RecordSet() {
	delete[] printWidth;
	delete[] attrType;
	for(int i =0;i<numOfRecords;i++){
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
	updatePrintWidth(rec->getValSize());
	numOfRecords++;
	return 1;
}

int RecordSet::addRecord(Record* record){
	records.push_back(record);
	updatePrintWidth(record->getValSize());
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

void RecordSet::updatePrintWidth(vector<int> valSize){
	for(int i=0;i<numOfAttr;i++){
		if(valSize[i]>printWidth[i]){
			printWidth[i] = valSize[i];
		}
	}
}

void RecordSet::printAll(int start, int limit){

	if(numOfRecords==0){
		cout << endl << "No Records Found."<<endl;
		return;
	}
	cout << endl<< numOfRecords<<" records found.";
	//cout << endl<< "Columns : " << numOfAttr;
	//cout << endl<< "Attribute Types : ";
	int i;
	/*for(i = 0;i<numOfAttr;i++){
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
	}*/
	string filler = "";
	int j=0;

	for(i=0;i<numOfAttr;i++){
		for(j=0;j<printWidth[i];j++)
			filler.append("_");
		filler.append("___");
	}
	cout << endl;
	cout << filler<<endl;
	if(colNameExist){
		for(i = 0;i<numOfAttr;i++){
			cout << setw(printWidth[i]+2) << left<<attrName[i]<<"|" ;
		}
	}else{
		for(i = 0;i<numOfAttr;i++){
			cout << setw(printWidth[i]+2) << left<< "Col"<<i+1 <<"|";
		}
	}
	cout <<endl<< filler<<endl;

	int end = (start+limit>numOfRecords)?numOfRecords:start+limit;
	vector<char*> val;

	for(i = start;i<end;i++){
		val = records[i]->getValues();
		for(j=0;j<numOfAttr;j++){
			cout << setw(printWidth[j]+2) << left<<val[j]<<"|";
		}
		cout << "\n";
	}

	cout <<filler<< "\n\n";

}


void RecordSet::printAllIndex(int start, int limit){

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
	//int end = start + limit;
	for(i = 0;i<numOfRecords;i++){
		vector<char*> val = records[i]->getValues();
		for(j=0;j<numOfAttr;j++){
			cout << val[j] << "\t\t";
		}
		cout << val[j] << "\t\t";
		cout << val[++j] << "\t\t";
		cout << endl;
	}
	cout << endl;

}

int RecordSet::getSizeOf(unsigned recNo){
	if(recNo < 0||recNo > records.size())
		return 0;
	return records[recNo]->getSize();
}

void RecordSet::setAttrName(vector<char*> aName, int numAttr, int* pos){
	if(numAttr == 0){
		attrName = aName;
	}else{
		for(int i=0;i<numAttr;i++){
			attrName.push_back(aName[pos[i]]);
		}
	}

	colNameExist = true;
	int len;
	for(unsigned i=0;i<attrName.size();i++){
		if((len=strlen(attrName[i]))>printWidth[i]){
			printWidth[i] = len;
		}
	}
}

int compare(char* v1, char* v2, int type){
	/*
	 * >=-1 v1 is less
	 * 0 equal
	 * 1<= v2 is less
	 * */
	switch(type){
	case 1:
		return strcmp(v1,v2);
	case 2:
		return atoi(v1)-atoi(v2);
	case 3:
		return atol(v1)-atol(v2);
	default :
		return -1;

	}

}

void RecordSet::sortResult(int attrID){
	//insertionSort
	//in sorting
	int i, j;
	//cout << "sorting";
	Record* tmp;
	int type = attrType[attrID];
	char* value;

	for (i=0; i<numOfRecords; i++)
	{
		j=i;
		tmp=records[i];
		value = tmp->getValue(attrID);
		//if(j>0) cout << "\n Comparing : "<<value<<","<<(records[j-1]->getValue(attrID))<<","<<type<<"\n";
		while (j>0 && compare(value,records[j-1]->getValue(attrID),type)<0)
		{
			//cout <<"in\n";
			records[j]=records[j-1];
			j--;
		}
		records[j]=tmp;

	}
}

void RecordSet::clearAllRecs(){
	numOfRecords = 0;
	records.clear();
}

} /* namespace datamodels */
