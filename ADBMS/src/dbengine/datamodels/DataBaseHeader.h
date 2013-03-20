/*
 * DataBaseHeader.h
 *
 *  Created on: Mar 20, 2013
 *      Author: latchu
 */

#ifndef DATABASEHEADER_H_
#define DATABASEHEADER_H_

namespace datamodels {

class DataBaseHeader :public Page{
	long noOfTables;
	long noOfPages;
public:
	DataBaseHeader();
	DataBaseHeader(bool newFile);
};

} /* namespace datamodels */
#endif /* DATABASEHEADER_H_ */
