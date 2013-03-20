/*
 * DirPage.cpp
 *
 *  Created on: Dec 25, 2012
 *      Author: latchu
 */

#include "DirPage.h"

namespace datamodels {

DirPage::DirPage() {
	cout << endl << "Creating Dir Page";
	nextPage = 0;
	prevPage = 0;
	maxSpaceAvailable = 0;
	DEcount = 0;
	//offset of the start of contiguous free space from 'c'
	//The Storage Area
	writeToPage();
	dirEntries = DirEntry::getAllEntries(p,DEcount);
	for(int i = 0;i<DEcount;i++){
		lg2("");
		lg2("DEs: "<< i<<" " << dirEntries[i]->getPageID() << " "<<dirEntries[i]->getTFS());

	}
	printDetails();
}

DirPage::DirPage(long pid):Page(pid){
	DirPage* dp;
	char* buf = new char[sizeof(DirPage)];
	memcpy(buf,p,sizeof(DirPage));
	dp = (DirPage*)buf;
	nextPage = dp->nextPage;
	prevPage = dp->prevPage;
	DEcount = dp->DEcount;
	maxSpaceAvailable = dp->maxSpaceAvailable;
	dirEntries = DirEntry::getAllEntries(p,DEcount);

	for(int i = 0;i<DEcount;i++){
		lg2("DEs: "<< i<<" " << dirEntries[i]->getPageID() << " "<<dirEntries[i]->getTFS());
	}

	printDetails();
}

long DirPage::getDEcount(){
	return DEcount;
}

vector<DirEntry*> DirPage::getDirEntries(){
	return dirEntries;
}

long DirPage::getMaxSpaceAvailable(){
	return maxSpaceAvailable;
}

DirPage::~DirPage() {
	// TODO Auto-generated destructor stub
	delete [] p;
}

DirPage* DirPage::getNextPage(){
	if(nextPage == 0)
		return NULL;
	return new DirPage(nextPage);
}
void DirPage::setNextPage(long pid){
	this->nextPage = pid;
}

DirPage* DirPage::getPrevPage(){
	if(prevPage == 0)
		return NULL;
	return new DirPage(prevPage);
}
void DirPage::setPrevPage(long pid){
	this->prevPage = pid;
}

void DirPage::writeToPage(){
	memcpy(p,this,sizeof(DirPage));
	writePage();
}

void DirPage::printDetails(){
	lg2("Directory Page");
	lg2("Page : "<< pageid);
	lg2("Prev Page : " << prevPage);
	lg2("Next Page : " << nextPage);
	lg2("Max space in a DE : "<<maxSpaceAvailable);
	lg2("Totals DEs : "<<DEcount);
	lg2("");
}

DataPage* DirPage::createDataPage(){
	//create a datapage.
	//get tfs and store it in DE
	if(DEcount == MAX_DE){
		error("No Free space to insert DE");
		return NULL;
	}
	DataPage *dp = new DataPage();
	long pid = dp->getPageid();
	long tfs = dp->getTotalFreeSize();
	DirEntry *de = new DirEntry(pid,tfs);
	dirEntries.push_back(de);
	de->writeDE(p,DEcount);
	DEcount++;
	if(tfs>maxSpaceAvailable)
		maxSpaceAvailable = tfs;
	writeToPage();
	return dp;
}

int DirPage::insertRecords(RecordSet* rs, int s){
	//return number of records inserted.
	int start = 0;
	long dpNum;//DataPage Num.
	int recLen;
	DataPage* dp;
	int sid,i = 0;
	int n = rs->getNumOfRecords();

	vector<Record*> rec = rs->getAllRecords();
	int *attrType = rs->getAttrType();

	for(i=s;i<n;i++){
		recLen = rec[i]->getSize();
		dpNum = findNextPartialPage(start,recLen);//find the first partial page
		if(dpNum == -1){
			//no sufficient page.
			//try to create new DP and add it,vector<DirEntry*> dirEntries to DE.
			if(DEcount < MAX_DE ){
				//create new DataPage.
				dp = createDataPage();
				start = DEcount - 1;
			}else{
				//further insertion not possible
				error("@DirPage Insertion : DirPage Full.");
				//return the number of records inserted.
				writeToPage();
				return i - s;
			}
		}else{
			dp = new DataPage(dirEntries[dpNum]->getPageID());
			start = dpNum;
		}
		sid = dp->insertRecord(rec[i]->toRecordString(attrType),rec[i]->getSize());
		if(sid!=-1){
			//successful insertion
			//TODO:use slotid to store somewhere.
			//update dirPage entries.
			lg("@DirPage Insertion: Record "<<i<<" - inserted in DataPage "<<dp->getPageid());

			dirEntries[start]->setTFS(dp->getTotalFreeSize());
			dirEntries[start]->writeDE(p,start);
			start = 0;//check from first again for next record.
			updateMaximumSpaceParam();
		}else{
			error("@DirPage Insertion : DataPage "<<dp->getPageid()<<" is full. Retrying next Page in DE");
			//insertion error
			//retry the insertion here again to select a diff DP.
			i--;//so the same record will be referenced again
			start++;//search from next page!
		}
		delete dp;
	}
	writeToPage();
	return i - s;
}

int DirPage::findNextPartialPage(int start,int recLen){
	for(int i = start;i<DEcount;i++)
		if(dirEntries[i]->getTFS()>=recLen)
			return i;
	return -1;
}

void DirPage::updateMaximumSpaceParam(){
	maxSpaceAvailable = 0;
	int i;
	for(i=0;i<DEcount;i++)
		if(maxSpaceAvailable < dirEntries[i]->getTFS())
			maxSpaceAvailable = dirEntries[i]->getTFS();
}

int DirPage::retrieveRecords(RecordSet* rs, Select *select, Where *where){

	if(DEcount == 0){
		lg2("@DirPage_"<<pageid<<" : No Records to retrieve.");
		return 0;
	}

	DataPage* datapage;

	long noOfRecs = 0;

	int dataPid;

	//	dirEntries;
	for(vector<DirEntry*>::size_type i = select->getStartDirEntryID();
			i<dirEntries.size() ; i++){
		//create DataPage for every entry and try retreiving records.
		dataPid = dirEntries[i]->getPageID();
		datapage = new DataPage(dataPid);
		noOfRecs += datapage->retrieveRecords(rs,select,where);
		if(select->isThereMore()){
			select->setStartDirEntryID(i);
			delete datapage;
			return noOfRecs;
		}
		delete datapage;
	}
	return noOfRecs;
}

int DirPage::deleteRecords(Where *where, int* attrType, int numOfAttr){
	if(DEcount == 0){
		lg2("@DirPage_"<<pageid<<" : No Records to delete.");
		return 0;
	}
	DataPage* datapage;

	long noOfRecs = 0;

	long dataPid;

	long maxTFS;

	DirEntry *de;
	//	dirEntries;
	for(vector<DirEntry*>::size_type i = 0;
			i<dirEntries.size() ; i++){
		//create DataPage for every entry and try retreiving records.
		de = dirEntries[i];
		dataPid = de->getPageID();
		datapage = new DataPage(dataPid);
		noOfRecs += datapage->deleteRecords(where,attrType,numOfAttr);
		maxTFS = datapage->getTotalFreeSize();
		de->setTFS(maxTFS);
		if(maxSpaceAvailable < maxTFS)
			maxSpaceAvailable = maxTFS;
		de->writeDE(p,i);
		writeToPage();
		delete datapage;
	}
	return noOfRecs;

}

long DirPage::updateRecords(Where *where, Modify *modify, RecordSet* rs){
	if(DEcount == 0){
		lg2("@DirPage_"<<pageid<<" : No Records to update.");
		return 0;
	}
	DataPage* datapage;

	long noOfRecs = 0;

	long dataPid;

	long maxTFS;

	DirEntry *de;
	//	dirEntries;
	for(vector<DirEntry*>::size_type i = modify->getStartDirEntry();
			i<dirEntries.size() ; i++){
		//create DataPage for every entry and try retreiving records.
		de = dirEntries[i];
		dataPid = de->getPageID();
		datapage = new DataPage(dataPid);
		noOfRecs += datapage->updateRecords(where,modify,rs);
		maxTFS = datapage->getTotalFreeSize();
		de->setTFS(maxTFS);
		if(maxSpaceAvailable < maxTFS)
			maxSpaceAvailable = maxTFS;
		de->writeDE(p,i);
		if(modify->isThereMore()){
			writeToPage();
			modify->setStartDirEntry(i);
			delete datapage;
			return noOfRecs;
		}
		delete datapage;
	}
	writeToPage();
	return noOfRecs;
}



/*
 *
 *
 * DirEntry definitions
 *
 *
 *
 */
DirEntry::DirEntry(){

}

DirEntry::DirEntry(long pid,long tfs){
	this->pageid = pid;
	this->tfs = tfs;
}

DirEntry::~DirEntry(){
}

int DirEntry::writeDE(char* buf, long id){
	memcpy(buf+sizeof(DirPage)+id*sizeof(DirEntry),this,sizeof(DirEntry));
	return 1;
}

vector<DirEntry*> DirEntry::getAllEntries(char *buf, long totalDEs){
	vector<DirEntry*> entries;
	char *loc = buf + sizeof(DirPage);
	char *temp;
	DirEntry *de;
	for(int i=0;i<totalDEs;i++){
		temp = new char[sizeof(DirEntry)];
		memcpy(temp,loc,sizeof(DirEntry));
		de = (DirEntry*)temp;
		entries.push_back(de);
		loc += sizeof(DirEntry);
	}
	return entries;
}

long DirEntry::getPageID(){
	return pageid;
}

long DirEntry::getTFS(){
	return tfs;
}

void DirEntry::setPageID(long pid){
	this->pageid = pid;
}
void DirEntry::setTFS(long tfs){
	this->tfs = tfs;
}


} /* namespace datamodels */

//using namespace datamodels;
//
//int main(){
////	DirPage *d = new DirPage(1);
//	//	for(int i=0;i<200;i++)
//	//		DataPage *dp = d->createDataPage();
//	//	dp->printDetails();
//	char *elements[19] = {
//			"JanuaryJanuaryJanuaryJanuary",
//			"FebruaryFebruaryFebruaryFebruary",
//			"MarchMarchMarchMarch",//3
//			"AprilAprilAprilApril",
//			"MayMayMayMayMayMayMayMayMayMayMayMay",
//			"JuneJuneJuneJuneJune",//6
//			"JulyJulyJulyJulyJuly",
//			"AugustAugustAugust",
//			"SeptemberSeptemberSeptemberSeptember",//9
//			"OctoberOctoberOctoberOctoberOctober",
//			"NovemberNovemberNovemberNovemberNovember",
//			"DecemberDecemberDecemberDecember",//12
//			"SundaySundaySundaySundaySundaySunday",
//			"MondayMondayMondayMonday",
//			"TuesdayTuesdayTuesdayTuesday",//15
//			"WednesdayWednesdayWednesdayWednesday",
//			"ThursdayThursdayThursdayThursday",
//			"FridayFridayFridayFriday",//18
//			"SaturdaySaturdaySaturdaySaturday"
//	};
//
////	int tr = 0,temp;
////
////	for(int i=0;i<10000;i++){
////		temp = d->insertRecords(elements,19);
////		if(temp == -1){
////			break;
////		}
////		tr += temp;
////	}
////
////	lg("");
////	lg("tr : "<<tr);
//
////	d->printDetails();
////
////	//	d->setNextPage(10);
////	//	d->writeToPage();
////
////		DataPage *dp = new DataPage(19);
////		dp->printAllRecords();
//char hello[] = "Hello";
//char *a; char*b;
//a = new char[10];
//b = new char[10];
//
//char h[] = "11111111111111111111111";
//memcpy(a,h,10);
//memcpy(b,h,10);
//
//memcpy(a,hello,strlen(hello));
//memcpy(b,hello,strlen(hello)+1);
//
//lg(hello);
//lg("a "<<a);
//lg("b "<<b);
//
//}
