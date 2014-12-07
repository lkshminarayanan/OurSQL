/*
 * RecordID.h
 *
 *  Created on: Dec 31, 2012
 *      Author: latchu
 */

#ifndef RECORDID_H_
#define RECORDID_H_
#include "globals.h"

using namespace datamodels;

namespace dbEngine {

class RecordID {
private:
	long pageId;
	long slotId;
	char* keyValue;
	int keyType;
	/*
	* 1 - string
	* 2 - int 
	* 3 - float
	*/
    static char keyTypeStr[3][7];
public:
	RecordID();
	RecordID(long pid, long slotid, char* keyValue, int kType);//during index insertion
	RecordID(long pid, long slotid);//during retrieval from the index
	
	long getPageNo();
	long getSlotID();
	char* getKeyValue();
	char* getKeyType();
	
	void setPageNo(long pid);
	void setSlotID(long slotid);
	void setKeyValue(char* key);
	void setKeyType(int ktype);

};

} /* namespace dataEngine */
#endif /* RECORDID_H_ */
