/*
 * mainFile.c
 *
 *  Created on: Mar 27, 2013
 *      Author: latchu
 */

#include "Table.h"

using namespace datamodels;
int main1(){

	/*Adding a Record from input
	 * Type : 1-char 2-int 3-real
	 *
	 * */
	Record *r = new Record();
	r->addValue("Lakshmi",1);
	r->addValue("2",2);
	r->addValue("3.4",3);



	Record *r1 = new Record();
	r->addValue("Narayanan ",1);
	r->addValue("21",2);
	r->addValue("3.5",3);


	int a[3] = {1,2,3};

	RecordSet *rs;

	rs = new RecordSet(a,3);

	rs->addRecord(r);
	rs->addRecord(r1);

	DataBaseServer *dbs = new DataBaseServer("iiitb",false);
	if(dbs->isValid()){

		//		vector<char*> aName;
		//		vector<int> maxSize, aType;
		//		//		aName.push_back("Name");  maxSize.push_back(30); aType.push_back(1);
		//		//		aName.push_back("Age");  maxSize.push_back(3); aType.push_back(2);
		//		//		aName.push_back("Grade"); maxSize.push_back(2); aType.push_back(3);
		//		//		aName.push_back("Comments"); maxSize.push_back(10); aType.push_back(1);
		//		//		Table *tbl = new Table("TEST", aName, maxSize, aType,dbs);
		//		dbs->showTables();
		//		cout << "Here"; cout.flush();
		Table* tbl = new Table("mtech",dbs);

		tbl->insertTuples(rs);
		rs = tbl->selectTuples();
		//	rs->printAll(0,100);
		rs->printAll(0,100);

		delete dbs;

	}else{
		cout<<"Invalid Name";
	}

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

int mainDebug(){
	int ch;
	char dbName[100];
	DataBaseServer *dbs = NULL;
	while(1){
		cout<<endl<<"1.Create Database";
		cout<<endl<<"2.Load Database";
		cout<<endl<<"3.Show Tables";
		cout<<endl<<"4.Create Table";
		cout<<endl<<"5.Insert to table";
		cout<<endl<<"6.Select *";
		cout<<endl<<"7.Select colnames";
		cout<<endl<<"";
		cout<<endl<<"0.Exit";
		cin>>ch;
		switch(ch){
		case 0:
			if(dbs != NULL)
				delete dbs;
			exit(0);
		case 1:
			cout << endl << "Enter Database Name";
			cin >> dbName;
			if(dbs != NULL)
				delete dbs;
			dbs = new DataBaseServer(dbName,true);
			if(dbs->isValid()){
				cout << dbName<<" Created";
			}else{
				cout <<"Error! Unable to create "<<dbName<<
						". Another db with same Name exists.";
			}
			break;
		case 2:
			cout << endl << "Enter Database Name";
			cin >> dbName;
			if(dbs != NULL)
				delete dbs;
			dbs = new DataBaseServer(dbName,false);
			if(dbs->isValid()){
				cout << dbName<<" Loaded";
			}else{
				cout <<  "Error!"<< dbName<<" does not exist.";
			}
			break;
		case 3:
			if(dbs == NULL){
				cout <<endl<< "Load DB first";
			}else{
				dbs->showTables();
			}
			break;
		case 4:
			if(dbs == NULL){
				cout <<endl<< "Load DB first";
			}else{
				int noOfattr,i,attrType;
				char *attrName,tableName[100];
				vector<char*> aName;
				vector<int> maxSize, aType;
				cout <<endl<<"TableName? :";
				cin>>tableName;
				cout <<endl<<"Enter num of Attr :";
				cin >> noOfattr;
				for(i=0;i<noOfattr;i++){
					attrName = new char[100];
					cout<<"Attr"<<i<<" Name? :";
					cin>>attrName;
					aName.push_back(attrName);
					maxSize.push_back(50);
					cout<<"Attr"<<i<<" Type?(1-char;2-int;3-real) :";
					cin>>attrType;
					aType.push_back(attrType);
				}
				Table *tbl = new Table(tableName, aName, maxSize, aType,dbs);
				if(tbl->getDirPageHeader()==NULL){
					cout << "Cannot create Table. Duplicate Table Name.";
				}else
					delete tbl;
			}
			break;
		case 5:
			if(dbs == NULL){
				cout <<endl<< "Load DB first";
			}else{
				int noOfattr,i;
				vector<int> attrType;
				vector<char*> attrName;
				char tableName[100];
				vector<char*> aName;
				vector<char*> values;
				char *val;
				cout <<endl<<"TableName? :";
				cin >> tableName;
				Table *tbl = new Table(tableName,dbs);
				if(tbl->getDirPageHeader()==NULL){
					cout << endl<<"Table doesnot exist.";
				}else{
					noOfattr = tbl->getNumOfAttr();
					//cout << "Total Attributes :"<<noOfattr;
					attrName = tbl->getAttributeNames();
					attrType = tbl->getAttributeTypes();
					for(i=0;i<noOfattr;i++){
						val = new char[100];
						cout << "Enter Values :";
						cout <<endl<< i << "-"<<attrName[i]<<"["<<attrType[i]<<"]: ";
						cin >> val;
						values.push_back(val);
					}
					//					char* t = attrName[1];
					//					attrName[1] = attrName[0];
					//					attrName[0] = t;
					tbl->insertTuples(attrName,values);
					delete tbl;
				}
			}

			break;
		case 6:
			if(dbs == NULL){
				cout <<endl<< "Load DB first";
			}else{
				char tableName[100];
				cout <<endl<<"TableName? :";
				cin >> tableName;
				Table *tbl = new Table(tableName,dbs);
				if(tbl->getDirPageHeader()==NULL){
					cout << endl<<"Table doesnot exist.";
				}else{
					RecordSet *rs = tbl->selectTuples();
					rs->printAll(0,3);
				}
			}
			break;
		case 7:
			if(dbs == NULL){
				cout <<endl<< "Load DB first";
			}else{
				char tableName[100];
				cout <<endl<<"TableName? :";
				cin >> tableName;
				Table *tbl = new Table(tableName,dbs);
				if(tbl->getDirPageHeader()==NULL){
					cout << endl<<"Table doesnot exist.";
				}else{
					int numOfCol;
					vector<char*> attrName = tbl->getAttributeNames();
					vector<char*> selAttrName;
					char *colName;
					int tableTotalCol = tbl->getNumOfAttr();
					cout << endl << "No. of columns to select :";
					cin>>numOfCol;
					cout <<endl<<"Choose the column [";
					for(int i=0;i<tableTotalCol;i++){
						cout <<" '"<<attrName[i]<<"' ";
					}
					cout << "]";
					for(int i=0;i<numOfCol;i++){
						colName = new char[100];
						cin >> colName;
						selAttrName.push_back(colName);
					}
					RecordSet *rs = tbl->selectTuples(1,selAttrName);
					rs->printAll(0,100);
				}
			}
			break;
		default:
			cout <<endl<< "Invalid Request";
			break;
		}
	}
	return 0;
}
