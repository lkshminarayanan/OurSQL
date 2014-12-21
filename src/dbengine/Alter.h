/*
 * Alter.h
 *
 *  Created on: May 31, 2013
 *      Author: latchu
 */

#ifndef ALTER_H_
#define ALTER_H_

#include "globals.h"
#include <vector>

using namespace datamodels;

namespace dbEngine {

class Alter {
private:
	vector<int> dataType;
	int attrId;
public:
	Alter();
	Alter(int attrID);
	Alter(vector<int> dataType);
	Alter(int attrID, int dataType);

	Record* alterAddColumn(Record *r);
	Record* alterDropColumn(Record *r);
	bool checkColumn(RecordSet *rs);
};

} /* namespace datamodels */
#endif /* ALTER_H_ */
