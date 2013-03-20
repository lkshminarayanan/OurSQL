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
	case 1:
		//		char *names[18] = {
		//				"TID","TABLE NAME", "No of Columns", "Records", "start Dir Page",  "Max COl Size"
		//		};
		//		for(int i = 0;i<6;i++)
		//			attrName.push_back("TID");
		//			attrName
		/* no break */
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
		numOfAttrColumns = 6;
		break;
	}
}

Table::Table(char* tName, DataBaseServer* db){
	//search in SYSCOlUMN and load the dirpage.
	//	RecordSet *rs = sys_tbl->selectTuples();
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
	}
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
}

Table::Table(char *tableName,vector<char*> attrName, vector<int> maxSize, vector<int> attrType, DataBaseServer* db){//creating new table

	//prepare a record about the table and insert into sys_table
	dbs = db;
	numOfAttrColumns = attrName.size();
	DirPage *dp = new DirPage();
	tID = dbs->addToSysTable(tableName,numOfAttrColumns,dp->getPageid());
	this->attrName = attrName;
	this->attrType = attrType;
	this->maxColSize = maxSize;

	for(int i = 0; i< numOfAttrColumns;i++){
		dbs->addToSysColumn(tID, attrName[i],i,maxSize[i],attrType[i],0);
	}

}

RecordSet* Table::selectTuples(){
	return selectTuples(new Select(),new Where());
}

RecordSet* Table::selectTuples(Select *select,Where *where){
	DirPage *dp;
	RecordSet *rs = new RecordSet(attrType,numOfAttrColumns);
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
	return recsInserted;
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
	DirPage* dp = tHeader, *old;
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

} /* namespace datamodels */

using namespace datamodels;
int main(){
//	Record *r = new Record();
//	r->addValue("9994",2);
//	r->addValue("8888",2);
//	r->addValue("443.99995",3);
//	r->addValue("lakshmi",1);
//
//
//
//	Record *r1 = new Record();
//	r1->addValue("443",2);
//	r1->addValue("9600",2);
//	r1->addValue("223.99995",2);
//	r1->addValue("Narayanan",2);
//
//
//	int a[4] = {2,2,3,1};
//	//
//	RecordSet *rs;
//	//
//	double x = 0;
//
	DataBaseServer *dbs = new DataBaseServer("sample",false);
//	vector<char*> aName;
//	vector<int> maxSize, aType;
//	aName.push_back("Name");  maxSize.push_back(30); aType.push_back(1);
//	aName.push_back("Age");  maxSize.push_back(3); aType.push_back(2);
//	aName.push_back("Grade"); maxSize.push_back(2); aType.push_back(3);
//	Table *tbl = new Table("Lakshmi Abi", aName, maxSize, aType,dbs);


	dbs->showTables();

//	Table *tbl = new Table("England",dbs);



	//			DirPage *dp = new DirPage(2);
	//			int a1[] = {2,1,2,2,2,2};
	//			rs = new RecordSet(a1,6);
	//			dp->retrieveRecords(rs,new Select(),new Where());
	//			rs->printAll(0,100);

	//	rs = new RecordSet(a1,6);
	//	DataPage *dap = new DataPage(5);
	//	dap->retrieveRecords(rs,new Select(),new Where());
	//	rs->printAll(0,100);



	//	DirPage *c = new DirPage();
	//	cout << c->getPageid();
	//	Table *tbl = new Table(1,false);

	/*	rs = new RecordSet(a,4);
		for(int i = 0 ;i<5;i++){
			rs->addRecord(r);
			rs->addRecord(r1);
		}

		for(int i = 0;i<4;i++){
			x += tbl->insertTuples(rs);
		}

		lg("Total Records : "<<x);*/
//	x = 0;
//	clock_t tStart = clock();
//
//	RecordSet *rr;
//	//
//	rr = tbl->selectTuples(new Select(),new Where());
//	x += rr->getNumOfRecords();
//	cout << endl << "Total Records : "<<x;
//
//	rr->printAll(0,100);
	//
	//
	//	printf("\nTime taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

	//	/*
	//	 * Delete Test Case
	//	 tbl->deleteTuples(w,a,4);
	//
	//
	//	x= 0;
	//
	//	do{
	//		rr = tbl->selectTuples(s,w);
	//		x += rr->getNumOfRecords();
	//		//		s->logDetails();
	//	}while(s->isThereMore());
	//	cout << endl << "Total Records : "<<x;
	//
	//
	//	 */
	//
	//	x = 0;
	//
	//	Modify * m = new Modify(a,4);
	//	m->addUpdatedValues("india",3);
	//
	//	w->addCondition(0,wc);
	//	w->addCondition(2,new WhereCond(3,1,0,"india"));
	//
	//	cout << endl << "Modified Records : "<<tbl->updateTuples(w,m);
	//
	//	do{
	//		rr = tbl->selectTuples(s,new Where());
	//		x += rr->getNumOfRecords();
	//		//		s->logDetails();
	//	}while(s->isThereMore());
	//	cout << endl << "Total Records : "<<x;
	//
	//	rr->printAll(0,100);

	return 1;
}
