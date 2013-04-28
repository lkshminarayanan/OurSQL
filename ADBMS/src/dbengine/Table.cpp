/*
 * Table.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "Table.h"

using namespace datamodels;

namespace dbEngine {

Table::Table() {}
Table::~Table() {}
Table::Table(int pid, bool createNew){
	if(createNew){
		tHeader = new DirPage();
	}else{
		tHeader = new DirPage(pid);
	}
	switch (pid) {
	case 1:/* no break */
	case 2:
		attrType.push_back(2);
		attrType.push_back(1);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		numOfAttrColumns = 6;
		break;
	default:
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		numOfAttrColumns = 3;
		break;
	}
}

Table::Table(char* tName, DataBaseServer* db){
	//search in SYSCOlUMN and load the dirpage.
	//	RecordSet *rs = sys_tbl->selectTuples();
	this->tName = new char[strlen(tName)];
	strcpy(this->tName,tName);

	WhereCond *wc = new WhereCond(1,1,0,tName);
	Where *w = new Where();
	w->addCondition(0,wc);
	RecordSet *rs = db->SysTable->selectTuples(new Select(),w);
	vector<Record*>  recs= rs->getAllRecords();
	//	rs->printAll(0,10);
	Record *r;
	long pid;
	vector<char*> sysTres;
	if(recs.size()!=0){
		r = recs[0];
		sysTres = r->getValues();
		pid = atol(sysTres[4]);
		//numOfRows = atol(sysTres[3]);
		tHeader = new DirPage(pid);
		dbs = db;

		WhereCond *wc2 = new WhereCond(0,2,0,sysTres[0]);
		Where *ww = new Where();
		ww->addCondition(0,wc2);

		rs = db->SysColumn->selectTuples(new Select(),ww);

		//	rs->printAll(0,rs->getNumOfRecords());

		numOfAttrColumns = atol(sysTres[2]);

		vector<Record*> sysCres = rs->getAllRecords();

		vector<char*> values;

		/*
		 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
		 */

		for(int i=0;i<numOfAttrColumns;i++){
			values = sysCres[i]->getValues();
			attrName.push_back(values[1]);
			attrType.push_back(atoi(values[4]));
			maxColSize.push_back(atoi(values[3]));
		}

		tID = atoi(sysTres[0]);
	}else{
		tHeader = NULL;
	}

}

Table::Table(char *tableName,vector<char*> attrName, vector<int> maxSize, vector<int> attrType, DataBaseServer* db){//creating new table
	//check if table with same name exists..
	RecordSet *rs;
	WhereCond *wc = new WhereCond(1,1,0,tableName);
	rs = db->SysTable->selectTuples(new Select(),new Where(wc));
	if(rs->getNumOfRecords()==0){
		//prepare a record about the table and insert into sys_table
		dbs = db;
		numOfAttrColumns = attrName.size();
		tHeader = new DirPage();
		//	cout << "Created DP :"<<dp->getPageid();
		tID = dbs->addToSysTable(tableName,numOfAttrColumns,tHeader->getPageid());
		this->attrName = attrName;
		this->attrType = attrType;
		this->maxColSize = maxSize;
		for(int i = 0; i< numOfAttrColumns;i++){
			dbs->addToSysColumn(tID, attrName[i],i,maxSize[i],attrType[i],0);
		}
	}else{
		tHeader = NULL;
	}

	this->tName = new char[strlen(tableName)];
	strcpy(this->tName,tableName);


}

RecordSet* Table::selectTuples(){
	return selectTuples(new Select(),new Where());
}

RecordSet* Table::selectTuples(Select *select,Where *where){
	DirPage *dp;
	RecordSet *rs;
	if(select->getNumOfAttr()==0){
		int pos[1];//dummy
		int *arrAttr = new int[numOfAttrColumns];

		for(int i = 0;i<numOfAttrColumns;i++){
			arrAttr[i] = attrType[i];
		}
		select = new Select(pos,0,arrAttr,numOfAttrColumns);
		rs = new RecordSet(attrType,numOfAttrColumns);
	}else{
		rs = new RecordSet(select->getAttrType(),select->getNumOfAttr());
	}

	if(tHeader->getPageid()>3)//valid attrNames oly for normal tables
		rs->setAttrName(attrName);

	long startDirPageID = select->getStartDirPageID();
	if(startDirPageID == 0)
		dp = tHeader;
	else
		dp = new DirPage(startDirPageID);
	select->setLimit(MAX_RETR_LIMIT);
	do{
		dp->retrieveRecords(rs,select,where);
		if(select->isThereMore()){
			select->setStartDirPageID(dp->getPageid());
			return rs;
		}else{
			dp = dp->getNextPage();
		}
	}while(dp!=NULL);
	return rs;
}

RecordSet* Table::selectTuples(int type,vector<char*> selectColumNames){
	int i,j;
	int selectColumns = 0;
	int *pos;
	int *attrTypeArr = new int[numOfAttrColumns];

	for(int i = 0;i<numOfAttrColumns;i++){
		attrTypeArr[i] = attrType[i];
	}
	switch(type){
	case 1://select
		selectColumns = selectColumNames.size();
		pos = new int[selectColumns];
		for(i=0;i<selectColumns;i++){
			for(j=0;j<numOfAttrColumns;j++){
				if(strcmp(attrName[j],selectColumNames[i])==0){
					pos[i] = j;
					break;
				}
			}
		}
		break;
	}
	return selectTuples(new Select(pos,selectColumns,attrTypeArr,numOfAttrColumns),new Where());
}

int Table::insertTuples(RecordSet *rs){
	DirPage *dp = tHeader;
	DirPage *next;
	int start = 0;
	int recsInserted = 0;
	bool newFlag;
	do{
		recsInserted += dp->insertRecords(rs,start);
		if(recsInserted < rs->getNumOfRecords()){
			//The Previous Dirpage is full now. create/load the next one.
			do{
				newFlag = false;
				next = dp->getNextPage();
				if(next == NULL){
					//create new DirPage
					next = new DirPage();
					dp->setNextPage(next->getPageid());
					next->setPrevPage(dp->getPageid());
					dp->writeToPage();
					next->writePage();
					newFlag = true;
				}
				//				delete dp;
				dp = next;
			}while(dp->getMaxSpaceAvailable()<rs->getSizeOf(start)&&!newFlag);
			start = recsInserted;
		}else{
			break;
		}
	}while(recsInserted < rs->getNumOfRecords());
	//TODO:update systable
	/*if(tHeader->getPageid()>=4){
		cout << "searching for "<<tName<<endl;
		Where* w = new Where(new WhereCond(1,1,0,tName));
		Modify* m = new Modify(rs->getAttrType(),numOfAttrColumns);
		numOfRows += recsInserted;
		char* inputChar = new char[sizeof(long)];
		sprintf(inputChar,"%ld",numOfRows);
		cout << "toup " << inputChar;
		m->addUpdatedValues(inputChar,3);
		cout << dbs->SysTable->updateTuples(w,m) << " rows updated";
	}*/
	return recsInserted;
}

int Table::insertTuples(vector<char*> aName,vector<char*> values){
	//check the order of the entry.
	int i,j;
	/*char **valuesArr =new char*[numOfAttrColumns];
	for(i = 0; i < numOfAttrColumns; ++i)
		valuesArr[i] = new char[100];*/
	int pos;
	Record *r = new Record();
	for(i=0;i<numOfAttrColumns;i++){
		pos = -1;
		for(j=0;j<aName.size();j++){
			if(strcmp(attrName[i],aName[j])==0){
				pos = j;
				break;
			}
		}
		if(pos!=-1){
			r->addValue(values[pos],attrType[i]);
		}else
			r->addValue("",attrType[i]);
	}

	RecordSet *rs;

	rs = new RecordSet(attrType,numOfAttrColumns);
	rs->addRecord(r);
	return insertTuples(rs);
}


long Table::deleteTuples(Where* where, int* attrType, int numOfAttr){
	DirPage* dp = tHeader;
	long noOfRecs = 0;
	while(dp != NULL){
		noOfRecs += dp->deleteRecords(where,attrType,numOfAttr);
		dp = dp->getNextPage();
	}
	//TODO: Update systable
	return noOfRecs;
}

long Table::updateTuples(Where *where,Modify* modify){
	DirPage* dp = tHeader;
	DirPage* old;
	long noOfRecs = 0;
	RecordSet *rs;
	while(dp != NULL){
		do{
			rs = new RecordSet(modify->getAttrType(),numOfAttrColumns);
			noOfRecs += dp->updateRecords(where,modify,rs);
			insertTuples(rs);
			modify->resetLimits();
		}while(modify->isThereMore());
		old = dp;
		dp = dp->getNextPage();
		//		delete old;
	}
	//TODO: Update systable
	return noOfRecs;
}

DirPage* Table::getDirPageHeader(){
	return this->tHeader;
}

int Table::getNumOfAttr(){
	return numOfAttrColumns;
}

vector<char*> Table::getAttributeNames(){
	return attrName;
}

vector<string> Table::getAttributeNamesString(){
	vector<string> attrNameS;
	for(int i =0;i<numOfAttrColumns;i++){
		attrNameS.push_back(attrName[i]);
	}
	return attrNameS;
}

long Table::getNumOfRows(){
	return numOfRows;
}

vector<int> Table::getAttributeTypes(){
	return attrType;
}

} /* namespace datamodels */

