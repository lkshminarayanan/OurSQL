/*
 * Page.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: latchu
 */

#include "Page.h"

namespace datamodels {

Page::Page() {
	Cache* cache = Cache::getInstance();
	p = new char[pageSize];
	pageid = cache->getNewPage();
	lg2( "Creating page : " << pageid);
}

Page::Page(long pid) {
	pageid = pid;
	Cache *cache = Cache::getInstance();
	char* buf = cache->readPage(pid);
	p = new char[pageSize];

	memcpy(p,buf,pageSize);

	lg2("Loading page : " << pageid);
}

Page::~Page() {
}

char* Page::getPage(){
	return p;
}

void Page::setPage(char* buf){
	this->p = buf;
}


Page* Page::readPage(long pid){
	Cache *cache = Cache::getInstance();
	Page *p = new Page(pid);
	p->p = cache->readPage(pid);
	return p;
}

int Page::writePage(){
	Cache *cache = Cache::getInstance();
	return cache->writePage(pageid,p);
}

long Page::getPageid() const {
	return pageid;
}
void Page::setPageid(long pageid) {
	this->pageid = pageid;
}

//char* Page::getNewPage(){
//	Cache* cache = Cache::getInstance();
//	return cache->getNewPage();
//}

} /* namespace datamodels */

