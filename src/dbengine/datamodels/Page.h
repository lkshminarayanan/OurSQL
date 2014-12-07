/*
 * Page.h
 *
 *  Created on: Dec 13, 2012
 *      Author: latchu
 */

#ifndef PAGE_H_
#define PAGE_H_
#include <iostream>
#include "Cache.h"
#include <string.h>
#include "global.h"
using namespace std;

namespace datamodels {

class Page {
protected:
	long pageid;
	char* p;

public:
	Page();
	Page(long);
	virtual ~Page();

	long getPageid() const;
	void setPageid(long);

	static Page* readPage(long);
	int writePage();

	char* getPage();
	void setPage(char*);

	char* getNewPage();

};

} /* namespace datamodels */
#endif /* PAGE_H_ */
