/*
 * Alter.cpp
 *
 *  Created on: May 31, 2013
 *      Author: latchu
 */

#include "Alter.h"

namespace dbEngine {

Alter::Alter() {}
Alter::Alter(int attrID) {
	this->attrId = attrID;
}

Alter::Alter(vector<int> dataType) {
	this->dataType=dataType;
}

Alter::Alter(int attrID, int dataType) {
	this->dataType.push_back(dataType);
	this->attrId = attrID;
}

Record* Alter::alterAddColumn(Record *r){
	for(unsigned i=0;i<dataType.size();i++){
		r->addValue("",dataType[i]);
	}
	return r;
}

Record* Alter::alterDropColumn(Record *r){
	vector<char*> values = r->getValues();
	int n = values.size();
	Record* r_new = new Record();
	for(int i=0;i<n;i++){
		if(i!=attrId){
			r_new->addValue(values[i],1);
		}
	}
	return r_new;
}

bool Alter::checkColumn(RecordSet *rs){
	return false;
}

} /* namespace datamodels */
