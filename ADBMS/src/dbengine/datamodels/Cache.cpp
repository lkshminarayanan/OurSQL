
#include "Cache.h"

namespace datamodels {

char* DBName;

Cache::Cache() {
	// TODO Auto-generated constructor stub
	string fn = DBName;
	string DBpath = "/home/latchu/Documents/ADBMS/"+fn+".db";
	if(isDBExists(DBpath)){
		db.open(DBpath.c_str(),ios::binary | ios::out | ios::in );
		char *buf = new char[pageSize];
		db.read(buf,pageSize);
		memcpy(&pageNum,buf,sizeof(pageNum));
		lg("DB Loaded. PageNum : " << pageNum);
	}else{//create new
		creat(DBpath.c_str(), S_IRWXU);
		db.open(DBpath.c_str(),ios::binary | ios::out | ios::in );
		pageNum = 1;
		char *buf = new char[pageSize];
		memcpy(buf,&pageNum,sizeof(pageNum));
		db.write(buf,pageSize);
		lg( "DB created");
	}
}

//Cache::Cache(char* filename){
//	DBName = filename;
//	Cache();
//}
//
int Cache::openFile(char* filename){
	DBName = filename;
	return 1;
}

Cache* Cache::cacheInstance = NULL;

Cache* Cache::getInstance(){
	if(cacheInstance == NULL)
		cacheInstance = new Cache();
	return cacheInstance;
}

Cache::~Cache() {
	// TODO Auto-generated destructor stub
	lg("Closing file");
	db.close();
}

int Cache::getNewPage(){
	char *buf = (char*)malloc(pageSize);
	memcpy(buf,&pageNum,sizeof(pageNum));
	db.seekp(pageSize*(pageNum),ios_base::beg);
	db.write(buf,pageSize);
	db.flush();
	pageNum++;
	//updating first page//maybe data-header
	db.seekp(0,ios_base::beg);
	char *buf1 = new char[pageSize];
	memcpy(buf1,&pageNum,sizeof(pageNum));
	db.write(buf1,pageSize);
	return pageNum - 1;
}

int Cache::writePage(long pageid, char* page){
	db.seekp(pageSize*pageid,ios_base::beg);
	lg2("Writing page : "<<pageid);
	db.write(page,pageSize);
	return 1;
}

char* Cache::readPage(long pageid){
	char *buf = new char[pageSize];
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

} /* namespace datamodels */
