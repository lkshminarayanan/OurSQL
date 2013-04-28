/*
 * Where.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "Where.h"

namespace dbEngine {

WhereCond::WhereCond() {}

WhereCond::WhereCond(int attrID, int attrType, int opID, char* value){
	this->attrID = attrID;
	this->op = opID;
	this->attrType = attrType;
	this->value = value;
}

WhereCond::~WhereCond() {
	// TODO Auto-generated destructor stub
}

bool WhereCond::testRecord(Record *r){
	char* data = r->getValues()[attrID];
	double res , data_v, value_v;
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
	default:
		break;
	}
	//TODO::a final touch
	return false;
}


Where::Where(){}
Where::Where(WhereCond* firstWhere){
	addCondition(0,firstWhere);
}
Where::~Where(){};

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
	return where.size();
}

int Where::isWhereReq(){
	return where.size();
}

} /* namespace datamodels */

using namespace datamodels;
using namespace dbEngine;

int main_w(){

	/*
	 * 0 =
	 * 1 >
	 * 2 <
	 * 3 <>
	 *
	 */

	Record *r1 = new Record();
	r1->addValue("99995",2);
	r1->addValue("88",2);
	r1->addValue("443.3199999999",3);
	r1->addValue("lakshmin",1);

	Record *r2 = new Record();
	r2->addValue("99995",2);
	r2->addValue("89",2);
	r2->addValue("443.3199999999",3);
	r2->addValue("lakshmina",1);

	Record *r3 = new Record();
	r3->addValue("99995",2);
	r3->addValue("90",2);
	r3->addValue("443.3199999999",3);
	r3->addValue("lakshmis",1);

	WhereCond *wc = new WhereCond(1,2,1,"90");
	WhereCond *wc1 = new WhereCond(3,1,0,"lakshmin");

	Where *w = new Where();

	w->addCondition(0,wc);
	w->addCondition(2,wc1);

	cout <<endl<< "Result "<< w->testRecord(r1);
	cout <<endl<< "Result "<< w->testRecord(r2);
	cout <<endl<< "Result "<< w->testRecord(r3);

	int a[4] = {2,2,3,1};

	RecordSet *rs = new RecordSet(a,4);

	rs->addRecord(r1);
	rs->addRecord(r2);
	rs->addRecord(r3);

	rs->printAll(0,3);

	int b[4] = {2,2,3,1};



	int pos[2] = {1,3};

	Select *s = new Select(pos,2,b,4);

	RecordSet *prs = new RecordSet(s->getAttrType(),2);

	cout << "recstr"<<((r1->toRecordString(b))+16);

	prs->addRecord(s->project(r1));
	prs->addRecord(s->project(r2));
	prs->addRecord(s->project(r3));

	prs->printAll(0,3);

	return 1;
}
