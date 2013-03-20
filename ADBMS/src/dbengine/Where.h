/*
 * Where.h
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#ifndef WHERE_H_
#define WHERE_H_

#include "datamodels/Record.h"
#include <stdlib.h>

using namespace datamodels;

using namespace std;

namespace dbEngine {

class WhereCond {
public:
	int attrID;//attribute loc in the table column
	int attrType;//Type of the attribute
	/*
	 * 1 - char
	 * 2 - int
	 * 3 - real
	 */
	int op;
	/*
	 * 0 =
	 * 1 >
	 * 2 <
	 * 3 <>
	 *
	 */
	char* value;
public:
	WhereCond();
	WhereCond(int attrID, int attrType,int operatorType,char* value);
	virtual ~WhereCond();

	bool testRecord(Record *r);//test the record here and send the result

};

class Where{

private:
	int operatorType[5];
	/*
	 * 0 - first constraint
	 * 1 - AND
	 * 2 - OR
	 * 3 - NOT
	 */
	vector<WhereCond*> where;
public:
	Where();
	~Where();

	bool testRecord(Record *r);
	int addCondition(int operatorType,WhereCond* whereCond);
	int isWhereReq();
};

} /* namespace datamodels */
#endif /* WHERE_H_ */
