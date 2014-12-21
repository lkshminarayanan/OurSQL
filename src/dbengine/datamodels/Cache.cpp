
#include "Cache.h"

int pageSize = 1024*4;

namespace datamodels {

char* DBName;

Cache::Cache(char* fileName, bool newFile) {
	string fileNameStr = fileName;
	string DBpath = fileNameStr+".db";

	if(!newFile){
		if(isDBExists(DBpath)){
			db.open(DBpath.c_str(),ios::binary | ios::out | ios::in );
			lg("DB Loaded.");
			pageNum = 0;
		}else{
			lg("Invalid Database Name");
			pageNum = -1;
		}
	}else{//create new
		if(isDBExists(DBpath)){
			//file already exists.cannot be created
			lg("Cannot create DB. Database Name Conflict");
			pageNum = -1;
		}else{
			creat(DBpath.c_str(), S_IRWXU);
			db.open(DBpath.c_str(),ios::binary | ios::out | ios::in );
			pageNum = 1;
			lg( "DB created");
		}
	}
}

Cache* Cache::cacheInstance = NULL;

int Cache::createCache(char* fileName, bool newFile){
	//called first time exclusively by database header!
	cacheInstance = new Cache(fileName,newFile);
	if(cacheInstance->pageNum == -1)
		return -1;
	return 1;
}

void Cache::setPageNum(long pid){
	pageNum = pid;
}

Cache* Cache::getInstance(){
	//called every other time
	return cacheInstance;
}

Cache::~Cache() {
	// TODO Auto-generated destructor stub
	lg("Closing file");

	db.close();
}

int Cache::getNewPage(){
	char *buf = new char[pageSize];
	long pNum;//page number
	memset( buf, '\0', sizeof(char)*pageSize );
	if(freePages.size()==0){
		pNum = pageNum;
		pageNum++;
	}else{
		pNum = freePages.front();
		freePages.erase (freePages.begin());
	}
	db.seekp(pageSize*(pNum),ios_base::beg);
	db.write(buf,pageSize);
	db.flush();
	delete[] buf;

	//cout << "Cache : Giving Out "<<pNum<<endl;

	return pNum;
}

int Cache::writePage(long pageid, char* page){
	db.seekp(pageSize*pageid,ios_base::beg);
	lg2("Writing page : "<<pageid);
	db.write(page,pageSize);
	return 1;
}

char* Cache::readPage(long pageid){
	char *buf = new char[pageSize];
	memset(buf,'\0',pageSize);
	db.seekg(pageSize*pageid,ios_base::beg);
	db.read(buf,pageSize);
	return buf;
}

char* Cache::readPageFromIndex(long pageid){
	char *buf = (char*)malloc(pageSize*sizeof(char));
	memset(buf,'\0',pageSize);
	db.seekg(pageSize*pageid,ios_base::beg);
	db.read(buf,pageSize);
	return buf;
}

bool Cache::isDBExists(string dbpath){
	struct stat buf;
	if (stat(dbpath.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

long Cache::getPageNum(){
	return pageNum;
}

void Cache::deletePage(long pageId){
	lg("Deleting Page "<<pageId);
	freePages.push_back(pageId);
}
long* Cache::getFreePages(){
	long sz = freePages.size();
	long *fp = new long[sz];
	for(int i=0;i<sz;i++){
		fp[i] = freePages[i];
	}
	return fp;

}
void Cache::setFreePages(long *fp,long sz){
	freePages.clear();
	for(int i=0;i<sz;i++){
		freePages.push_back(fp[i]);
	}
}

long Cache::getCountFreePages(){
	return freePages.size();
}

} /* namespace datamodels */
