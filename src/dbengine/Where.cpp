/*
 * Where.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "Where.h"

namespace dbEngine {

WhereCond::WhereCond() {
	this->attrID = -1;
}

WhereCond::WhereCond(int attrID, int attrType, int opID, char* value){
	this->attrID = attrID;
	this->op = opID;
	this->attrType = attrType;

	char *data = new char[strlen(value)+1];
	strcpy(data,value);

	this->value = data;
}

WhereCond::WhereCond(int attrID, int attrType, int opID, long value){
	this->attrID = attrID;
	this->op = opID;
	this->attrType = attrType;
	char *data = new char[25];
	sprintf(data,"%ld",value);
	this->value = data;
}

WhereCond::WhereCond(int attrID, int attrType, int opID, string value){
	this->attrID = attrID;
	this->op = opID;
	this->attrType = attrType;
	char *data = new char[value.size()+1];
	strcpy(data,value.c_str());
	this->value = data;
}

WhereCond::~WhereCond() {
	// TODO Auto-generated destructor stub
	delete [] value;
}

void WhereCond::toString(){
	cout << "AttrID: "<<attrID;
	cout << "\nop: "<<op;
	cout << "\nattrType: "<<attrType;
	cout << "\nvalue: " <<value << endl;
}

bool WhereCond::testRecord(Record *r){
	char* data = r->getValues()[attrID];
	double res = -1 , data_v, value_v;
	switch (attrType) {
	case 1://char*
		res = strcmp(data,value);

		break;
	case 2://int
		data_v = atol(data);
		value_v= atol(value);
		res = data_v - value_v;
		break;
	case 3://real
		data_v = atof(data);
		value_v= atof(value);
		res = data_v - value_v;
		break;
	default:
		break;
	}
	switch (op) {
	case 0://=
		if(res == 0)
			return true;
		break;
	case 1://>
		if(res > 0 )
			return true;
		break;
	case 2://<
		if(res < 0)
			return true;
		break;
	case 3://<>
		if(res != 0){
			return true;
		}
		break;
	case 4://>=
		if(res > 0 ||res ==0)
			return true;
		break;
	case 5://<=
		if(res < 0 ||res ==0)
			return true;
		break;
	default:
		break;
	}
	//TODO::a final touch
	return false;
}


Where::Where(){
	numOfCond = 0;
}
Where::Where(WhereCond* firstWhere){
	numOfCond = 0;
	addCondition(0,firstWhere);
}
Where::~Where(){
	for(int i=0;i<numOfCond;i++){
		delete where[i];
	}
};

bool Where::testRecord(Record *r){

	if(where.size()==0){
		return true;
	}

	vector<Where*>::size_type i;
	bool retValue = false;
	for(i=0;i<where.size();i++){
		switch (operatorType[i]) {
		case 0:
			retValue = where[i]->testRecord(r);
			break;
		case 1:
			retValue &= where[i]->testRecord(r);
			break;
		case 2:
			retValue |= where[i]->testRecord(r);
			break;
		case 3:
			retValue = ! where[i]->testRecord(r);
			break;
		default:
			break;
		}
	}
	return retValue;
}

int Where::addCondition(int opType,WhereCond *whereCond){
	operatorType[where.size()]=opType;
	where.push_back(whereCond);
	numOfCond ++;
	return where.size();
}

int Where::isWhereReq(){
	return where.size();
}

void Where::toString(){
	for(unsigned int i=0;i<where.size();i++){
		cout << "\nCond-"<<i;
		cout << "\nAND/OR/NOT: "<<operatorType[i];
		cout << "\n";
		where[i]->toString();
	}
}

int Where::getNumOfCond(){
	return numOfCond;
}

int Where::getFirstCondAttr(){
	return where[0]->attrID;
}

char* Where::getFirstCondValue(){
	return where[0]->value;
}

int Where::getFirstCond(){
	return where[0]->op;
}

} /* namespace datamodels */

