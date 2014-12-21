/*
 * Table.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#include "Table.h"

bool indexStatus = false;

using namespace datamodels;

namespace dbEngine {

Table::Table() {}
Table::~Table() {
	if (tName!= NULL){
		delete [] tName;
		for(int i=0;i<numOfAttrColumns;i++){
			delete [] attrName[i];
		}
	}

	delete tHeader;
}
Table::Table(int pid, bool createNew){
	/*
	 * SysTable : tID, tName, noOfAttr, recs, startDirPage, maxColSize : 2, 1, 2, 2, 2, 2
	 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
	 * SysIndex : tID, colID, startPage, indexName : 2, 2, 2, 1
	 */
	if(createNew){
		tHeader = new DirPage();
	}else{
		tHeader = new DirPage(pid);
	}
	switch (pid) {
	case 1:/* no break */
		attrName.push_back("Table ID");
		attrName.push_back("Name");
		attrName.push_back("No Of Attr.");
		attrName.push_back("No of Recs");
		attrName.push_back("Start Dir Page");
		attrName.push_back("Max Col Size");

		attrType.push_back(2);
		attrType.push_back(1);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);

		numOfAttrColumns = 6;
		break;
	case 2:
		attrName.push_back("Table ID");
		attrName.push_back("Col Name");
		attrName.push_back("col ID");
		attrName.push_back("Max Size");
		attrName.push_back("Attr Type");
		attrName.push_back("is Indexed?");

		attrType.push_back(2);
		attrType.push_back(1);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);

		numOfAttrColumns = 6;
		break;
	case 3:
		attrName.push_back("Table ID");
		attrName.push_back("Col ID");
		attrName.push_back("MetaPage Start");
		attrName.push_back("Idx Name");

		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(2);
		attrType.push_back(1);

		numOfAttrColumns = 4;
		break;
	}
	tName = NULL;
}

Table::Table(char* tName, DataBaseServer* db){
	//search in SYSCOlUMN and load the dirpage.
	//	RecordSet *rs = sys_tbl->selectTuples();

	WhereCond *wc = new WhereCond(1,1,0,tName);
	Where *w = new Where();
	Select *s;
	w->addCondition(0,wc);
	s = new Select();
	RecordSet *rs = db->SysTable->selectTuples(s,w);
	vector<Record*>  recs= rs->getAllRecords();
	//	rs->printAll(0,10);
	Record *r;
	long pid;
	vector<char*> sysTres;
	if(recs.size()!=0){
		r = recs[0];
		sysTres = r->getValues();
		pid = atol(sysTres[4]);
		numOfRows = atol(sysTres[3]);
		numOfAttrColumns = atol(sysTres[2]);
		tHeader = new DirPage(pid);
		tID = atoi(sysTres[0]);
		dbs = db;

		this->tName = new char[strlen(tName)+1];
		strcpy(this->tName,tName);


		WhereCond *wc2 = new WhereCond(0,2,0,sysTres[0]);
		Where *ww = new Where();
		ww->addCondition(0,wc2);

		delete rs;
		delete s;

		s = new Select();
		rs = db->SysColumn->selectTuples(s,ww);
		rs->sortResult(2);

		delete ww;

		vector<Record*> sysCres = rs->getAllRecords();

		vector<char*> values;

		/*
		 * SysColumn : tID, colName , colID, maxSize, type, index : 2, 1, 2, 2, 2, 2
		 */

		char *attrNameTbl;

		for(int i=0;i<numOfAttrColumns;i++){
			values = sysCres[i]->getValues();

			attrNameTbl = new char[strlen(values[1])+1];
			strcpy(attrNameTbl,values[1]);
			attrName.push_back(attrNameTbl);

			attrType.push_back(atoi(values[4]));
			maxColSize.push_back(atoi(values[3]));
		}

		//logDetails();
	}else{
		tHeader = NULL;
		this->tName = NULL;
	}

	delete s;
	delete rs;
	delete w;

	opStatus = 0;
}

Table::Table(char *tableName,vector<char*> attrName, vector<int> maxSize, vector<int> attrType, DataBaseServer* db){//creating new table
	//check if table with same name exists..
	RecordSet *rs;
	WhereCond *wc = new WhereCond(1,1,0,tableName);
	Select* select = new Select();
	Where *where = new Where(wc);
	rs = db->SysTable->selectTuples(select,where);
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
		this->tName = new char[strlen(tableName)+1];
		strcpy(this->tName,tableName);
	}else{
		tHeader = NULL;
		tName = NULL;
	}

	delete rs;

	delete select;
	delete where;

	opStatus = 0;
	numOfRows = 0;
}

RecordSet* Table::selectTuples(){
	return selectTuples(new Select(),new Where());
}

RecordSet* Table::selectTuples(Select *select,Where *where){
	DirPage *dp, *old;
	RecordSet *rs;
	if(select->getNumOfAttr()==0){
		select->updateParamsForSelectStar(0,attrType,numOfAttrColumns);
		rs = new RecordSet(attrType,numOfAttrColumns);

		int pos[1];//dummy
		rs->setAttrName(attrName,0,pos);
	}else{
		rs = new RecordSet(select->getAttrType(),select->getNumOfAttr());
		rs->setAttrName(attrName,select->getNumOfAttr(),select->getProjectPos());
	}

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
			if(dp!=tHeader) delete dp;
			return rs;
		}else{
			old = dp;
			dp = dp->getNextPage();
			if(old!=tHeader) delete old;
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
				if(dp!=tHeader) delete dp;
				dp = next;
			}while(dp->getMaxSpaceAvailable()<rs->getSizeOf(start)&&!newFlag);
			start = recsInserted;
		}else{
			break;
		}
	}while(recsInserted < rs->getNumOfRecords());

	if(dp!=tHeader) delete dp;

	//update systable
	if(tHeader->getPageid()>=4&&opStatus!=3){
		Where* w = new Where(new WhereCond(1,1,0,tName));
		Modify* m = new Modify(dbs->SysTable->getAttributeTypes(), dbs->SysTable->getNumOfAttr());
		numOfRows += recsInserted;
		m->addUpdatedValues(numOfRows,3);
		dbs->SysTable->updateTuples(w,m);
		delete w;
		delete m;
	}
	return recsInserted;
}

int Table::insertTuples(vector<char*> aName,vector<char*> values){
	//check the order of the entry.
	int i;
	/*char **valuesArr =new char*[numOfAttrColumns];
	for(i = 0; i < numOfAttrColumns; ++i)
		valuesArr[i] = new char[100];*/
	int pos;
	Record *r = new Record();
	for(i=0;i<numOfAttrColumns;i++){
		pos = -1;
		for(unsigned j=0;j<aName.size();j++){
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

long Table::deleteTuples(Where* where, vector<int> attrType, int numOfAttr){
	int* attrType_arr = new int[attrType.size()];
	for(unsigned i=0;i<attrType.size();i++){
		attrType_arr[i] = attrType[i];
	}
	return deleteTuples(where,attrType_arr,numOfAttr);
}

long Table::deleteTuples(Where* where, int* attrType, int numOfAttr){
	DirPage* dp = tHeader;//TODO:
	long noOfRecs = 0;
	while(dp != NULL){
		noOfRecs += dp->deleteRecords(where,attrType,numOfAttr);
		dp = dp->getNextPage();
	}
	//TODO: Update systable
	return noOfRecs;
}

long Table::deleteTuples(Where *where){
	int* attrType_arr = new int[attrType.size()];
	for(unsigned i=0;i<attrType.size();i++){
		attrType_arr[i] = attrType[i];
	}

	DirPage* dp = tHeader;
	long noOfRecs = 0;
	while(dp != NULL){
		noOfRecs += dp->deleteRecords(where,attrType_arr,numOfAttrColumns);
		dp = dp->getNextPage();
	}

	//TODO: Update systable
	if(tHeader->getPageid()>=4){
		Where* w = new Where(new WhereCond(1,1,0,tName));
		Modify* m = new Modify(dbs->SysTable->getAttributeTypes(), dbs->SysTable->getNumOfAttr());
		numOfRows -= noOfRecs;
		m->addUpdatedValues(numOfRows,3);
		dbs->SysTable->updateTuples(w,m);
		delete w;
		delete m;
	}

	delete[] attrType_arr;
	return noOfRecs;
}

long Table::updateTuples(Where *where,Modify* modify){
	DirPage* dp = tHeader;
	DirPage* old;
	long noOfRecs = 0;
	RecordSet *rs;

	opStatus = 3;

	int updatedRecs;

	while(dp != NULL){
		do{
			rs = new RecordSet(attrType,numOfAttrColumns);
			updatedRecs = dp->updateRecords(where,modify,rs);
			//cout << "RS returned : "<<rs->getNumOfRecords()<<" -- "<<noOfRecs-prevrecs;
			//cout << " -- inserted : " <<
			if(updatedRecs > 0){
				insertTuples(rs);
				noOfRecs += updatedRecs;
			}

			modify->resetLimits();
			delete rs;
		}while(modify->isThereMore());
		old = dp;
		dp = dp->getNextPage();
		if(tHeader!=old) delete old;
	}
	return noOfRecs;
}

int Table::getColumnPos(string colName){
	//vector<string>::const_iterator itPos;
	auto itPos = std::find( attrName.begin() ,attrName.end(),colName);
	if(itPos == attrName.end()){
		return -1;
	}
	return itPos-attrName.begin();
}

DirPage* Table::getDirPageHeader(){
	return this->tHeader;
}

int Table::getNumOfAttr(){
	return numOfAttrColumns;
}

char* Table::getTableName(){
	return tName;
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

int Table::deleteTable(){

	DirPage *dp, *old;
	Cache *cache = Cache::getInstance();
	long startDirPageID = tHeader->getPageid();
	if(startDirPageID <= 3 )
		return 0;
	else{
		dp = new DirPage(startDirPageID);
		do{
			dp->deleteAll();
			cache->deletePage(dp->getPageid());
			old = dp;
			dp = old->getNextPage();
			delete old;
		}while(dp!=NULL);
		//TODO:update sys
		char *tidC = new char[10];
		sprintf(tidC,"%ld",tID);
		Where *where = new Where(new WhereCond(0,2,0,tidC));
		dbs->SysTable->deleteTuples(where);
		dbs->SysColumn->deleteTuples(where);
		dbs->SysIndex->deleteTuples(where);

		delete dp;
		delete where;
		delete[] tidC;
		return 1;
	}
}

int Table::alterTable(string tblName){
	/*renaming table*/
	//check for table;
	char *newName = new char[tblName.size()+1];
	strcpy(newName,tblName.c_str());


	Where *where = new Where(new WhereCond(1,1,0,newName));
	Select *select = new Select();
	RecordSet *rs = dbs->SysTable->selectTuples(select,where);

	delete select;
	delete where;

	if(rs->getNumOfRecords()!=0){
		cout << "Table name already exists.\n";
		return 0;
	}

	Modify *modify = new Modify(dbs->SysTable->attrType,dbs->SysTable->numOfAttrColumns);
	modify->addUpdatedValues(newName,1);

	where = new Where(new WhereCond(1,1,0,tName));
	int response = dbs->SysTable->updateTuples(where,modify);

	delete where;
	delete modify;
	delete[] newName;
	delete rs;

	return response;
}

int Table::alterTable(string oldName, string newName){
	/*renaming column name*/

	int response;

	char *oldNameC = new char[oldName.size()+1];
	strcpy(oldNameC,oldName.c_str());

	char *newNameC = new char[newName.size()+1];
	strcpy(newNameC,newName.c_str());

	if(getColumnPos(oldName)==-1){
		cout << "Column doesn't exist.\n";
		return 0;
	}

	//Checking
	if(getColumnPos(newName)!=-1){
		cout << "Column name already exists.\n";
		return 0;
	}

	Where *where = new Where(new WhereCond(0,2,0,tID));
	where->addCondition(1,new WhereCond(1,1,0,oldNameC));

	Modify *modify = new Modify(dbs->SysColumn->attrType,dbs->SysColumn->numOfAttrColumns);
	modify->addUpdatedValues(newNameC,1);

	response = dbs->SysColumn->updateTuples(where,modify);

	delete where;
	delete modify;
	delete[] newNameC;
	delete[] oldNameC;

	return response;
}

int Table::alterTable(vector<string> colName,vector<int> type,vector<int> maxSize){
	int i, n;
	n = colName.size();

	Alter *alter = new Alter(type);

	char *bkupTblName = new char[strlen(tName)+2];
	strcpy(bkupTblName,tName);
	strcat(bkupTblName,"$");

	//check if the columns exists.
	for(i=0;i<n;i++){
		if(getColumnPos(colName[i])!=-1){
			cout << "Colname already exists.\n";
			return 0;
		}
	}

	vector<Record*> recordList;
	Where *where = new Where(new WhereCond(1,1,0,tName));
	Select *select;

	//renaming the old table by appending an $
	Modify *modify = new Modify(dbs->SysTable->attrType,dbs->SysTable->numOfAttrColumns );
	modify->addUpdatedValues(bkupTblName ,1);
	dbs->SysTable->updateTuples(where,modify);
	delete modify;
	delete where;

	//creating types and names for new table;
	//copy existing columns first
	vector<char*> attrName_new;
	vector<int> attrType_new;
	vector<int> maxSize_new;
	char *aName;

	for(int i =0;i<numOfAttrColumns;i++){
		aName = new char[strlen(attrName[i])+1];
		attrName_new.push_back(strcpy(aName,attrName[i]));
		attrType_new.push_back(attrType[i]);
		maxSize_new.push_back(maxColSize[i]);
	}

	//adding new column names and types;
	for(i=0;i<n;i++){
		aName = new char[colName[i].size()+1];
		strcpy(aName,colName[i].c_str());
		attrName_new.push_back(aName);
		attrType_new.push_back(type[i]);
		maxSize_new.push_back((maxSize[i]==-1)?MAX_COLSIZE:maxSize[i]);
	}

	//create the table;
	Table *newTable = new Table(tName,attrName_new,maxSize_new,attrType_new,dbs);

	//rename this table;
	delete[] tName;
	tName = bkupTblName;

	//add index if there is an index
	select = new Select();
	where = new Where(new WhereCond(0,2,0,tID));
	RecordSet *rs = dbs->SysIndex->selectTuples(select,where);
	delete select;
	delete rs;
	delete where;

	RecordSet *alteredRS;

	select = new Select();
	where = new Where();
	do{
		alteredRS = new RecordSet(newTable->getAttributeTypes(),newTable->getNumOfAttr());
		rs = selectTuples(select,where);
		n = rs->getNumOfRecords();
		//rs->printAll(0,100);
		recordList = rs->getAllRecords();
		for(i=0;i<n;i++){
			alteredRS->addRecord(alter->alterAddColumn(recordList[i]));
		}

		newTable->insertTuples(alteredRS);

		delete alteredRS;
		rs->clearAllRecs();//because both rs are linked.
		delete rs;

	}while(select->isThereMore());

	deleteTable();


	/*//updating sysindex
	rs = dbs->SysIndex->selectTuples(new Select(),new Where(new WhereCond(0,2,0,tID)));

	if(rs->getNumOfRecords()>0){
		modify = new Modify(dbs->SysIndex->attrType,dbs->SysIndex->numOfAttrColumns );
		modify->addUpdatedValues(newTID,0);

		rsNew = new RecordSet(dbs->SysIndex->attrType,dbs->SysIndex->numOfAttrColumns);
		nk = rs->getNumOfRecords();
		recordList = rs->getAllRecords();
		for(i=0;i<nk;i++){
			rsNew->addRecord(modify->updateRecord(recordList[i]));
		}

		dbs->SysIndex->insertTuples(rsNew);
	}

	delete rs;*/

	delete newTable;
	delete select;
	delete where;
	delete alter;

	return 1;
}

int Table::alterTableDropColumn(string colName){

	int i, n,attrID;

	attrID = getColumnPos(colName);

	RecordSet *rs;
	Select *select;

	if(attrID==-1){
		cout << "Column does not exists.\n";
		return 0;
	}

	char *bkupTblName = new char[strlen(tName)+2];
	strcpy(bkupTblName,tName);
	strcat(bkupTblName,"$");

	Alter *alter = new Alter(attrID);

	//modifying systable entries
	vector<Record*> recordList;
	Where *where = new Where(new WhereCond(1,1,0,tName));
	//renaming the old table by appending an $
	Modify *modify = new Modify(dbs->SysTable->attrType,dbs->SysTable->numOfAttrColumns );
	modify->addUpdatedValues(bkupTblName ,1);
	dbs->SysTable->updateTuples(where,modify);
	delete modify;
	delete where;

	//Creating the new table with modified entries;
	vector<char*> attrName_new;
	vector<int> attrType_new;
	vector<int> maxSize_new;
	char *aName;

	for(int i =0;i<numOfAttrColumns;i++){
		if(i!=attrID){
			aName = new char[strlen(attrName[i])+1];
			attrName_new.push_back(strcpy(aName,attrName[i]));
			attrType_new.push_back(attrType[i]);
			maxSize_new.push_back(maxColSize[i]);
		}
	}


	//creating the new table
	Table *newTable = new Table(tName,attrName_new,maxSize_new,attrType_new,dbs);

	//renaming old table
	delete[] tName;
	tName = bkupTblName;

	//add index if there is an index
	select = new Select();
	where = new Where(new WhereCond(0,2,0,tID));
	rs = dbs->SysIndex->selectTuples(select,where);
	delete select;
	delete rs;
	delete where;

	//start copying tuples to other table;
	RecordSet *alteredRS;

	select = new Select();
	where = new Where();
	do{
		alteredRS = new RecordSet(newTable->getAttributeTypes(),newTable->getNumOfAttr());
		rs = selectTuples(select,where);
		n = rs->getNumOfRecords();
		//cout<<"before"<<endl;rs->printAll(0,100);
		recordList = rs->getAllRecords();
		for(i=0;i<n;i++){
			alteredRS->addRecord(alter->alterDropColumn(recordList[i]));
		}
		//cout<<"after"<<endl;alteredRS->printAll(0,10);

		newTable->insertTuples(alteredRS);

		delete alteredRS;
		delete rs;
	}while(select->isThereMore());

	//deleting this table finally.
	delete select;
	delete where;
	delete newTable;
	delete alter;

	deleteTable();

	return 1;
}

void Table::logDetails(){
	cout << tName<<endl;
	cout << "Tble ID : "<<tID<<endl;
	cout << "No of Attr : "<<numOfAttrColumns<<endl;
	cout << "No of Recs : "<<numOfRows<<endl;
	cout << "Start Dir Page : "<<tHeader->getPageid()<<endl;
}

char* Table::getMemCharfromInt(char *val){
	long intVal = atol(val);
	char* indexVal = new char[sizeof(intVal)];
	memcpy(indexVal,&intVal,sizeof(intVal));
	return indexVal;
}
char* Table::getMemCharfromDouble(char* val){
	float intVal = atol(val);
	char* indexVal = new char[sizeof(intVal)];
	memcpy(indexVal,&intVal,sizeof(intVal));
	return indexVal;
}
char* Table::getMemCharfromChar(char* val){
	char *indexCopy = new char[strlen(val)+1];
	return strcpy(indexCopy,val);

}

} /* namespace datamodels */
