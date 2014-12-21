/*
 * Parser.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: latchu
 */

#include "Parser.h"

namespace dbEngine {

static std::string toLowerCase( const std::string& in )
{
	std::string out;

	std::transform( in.begin(), in.end(), std::back_inserter( out ), ::tolower );
	return out;
}

Parser::Parser() {
	dbs = NULL;
}


Parser::~Parser() {
	if(dbs != NULL)
		delete dbs;
}

int Parser::str2int (string s)
{
	char  c;
	int i;
	std::stringstream ss(s);
	ss >> i;
	if (ss.fail() || ss.get(c)) {
		// not an integer
		return -1;
	}
	return i;
}

bool Parser::isAlpha(string s){
	int n = s.size();
	if((!isalpha(s[0]))&&s[0]!='_')
		return false;
	for(int i=1;i<n;i++)
		if((!isalpha(s[i]))&&s[i]!='_'&&(!isdigit(s[i])))
			return false;
	return true;
}

bool Parser::isInt(string s){
	int n = s.size();
	int i = 0;
	if((s[i]=='+'||s[i]=='-')&&n>1){
		i++;
	}
	for(;i<n;i++)
		if((!isdigit(s[i])))
			return false;
	return true;
}

bool Parser::isReal(string s){
	int n = s.size();
	int i = 0;
	int dot = 0;
	if((s[i]=='+'||s[i]=='-')&&n>1){
		i++;
	}
	for(;i<n;i++){
		if((!isdigit(s[i]))){
			if(s[i]=='.'&&dot==0){
				dot++;
			}else
				return false;
		}
	}
	return true;
}

string Parser::findType(string s){
	if(s.compare("int")==0||s.compare("integer")==0){
		return "2";
	}else if(s.compare("float")==0||s.compare("real")==0){
		return "3";
	}else if(s.compare("varchar")==0){
		return "1";
	}

	return "";
}

int Parser::parse(string stmt){

	if((stmt.size()==0||stmt.at(stmt.size()-1)!=';')||stmt.size()<2){
		cmderror;
		return 1;
	}

	stringstream ss (stmt.substr(0,stmt.size()-1));
	string item;
	vector<string> tokens;

	while(std::getline(ss, item))
	{
		std::size_t prev = 0, pos;
		bool readingVarchar = false;
		while ((pos = item.find_first_of(" ,()<=>'", prev)) != std::string::npos)
		{
			if(readingVarchar){
				tokens[tokens.size()-1].append(item.substr(prev, pos-prev));
				tokens[tokens.size()-1].append(item.substr(pos,1));
				if(item[pos]=='\''){
					readingVarchar = false;
				}

			}else{
				if (pos > prev)
					tokens.push_back(toLowerCase(item.substr(prev, pos-prev)));

				if(item[pos]=='\''){
					readingVarchar = true;
				}

				if(item[pos]!=' ')
					tokens.push_back(item.substr(pos, 1));
			}
			prev = pos+1;
		}
		if (prev < item.length()){
			if(readingVarchar){
				tokens[tokens.size()-1].append(item.substr(prev, std::string::npos));
			}else{
				tokens.push_back(toLowerCase(item.substr(prev, std::string::npos)));
			}

		}
	}

	try
	{
		if(tokens.at(0).compare("create")==0){

			if(tokens.at(1).compare("database")==0){
				if(isAlpha(tokens.at(2))&&tokens.size()==3)
					actUpon(1,tokens, false, false, -1);
				else{
					cmderror;
					return 1;
				}
			}

			else if(tokens.at(1).compare("table")==0){
				//create table tblname (col1 col1type[],col2 col2type[],..);
				if(!isAlpha(tokens.at(2))){
					cmderror;
					return 1;
				}else
					actUpon(4,tokens, true, false, -1);

			}

			else{
				cmderror;
			}
		}


		else if(tokens.at(0).compare("use")==0&&isAlpha(tokens.at(1))){
			if(tokens.size()>2){
				cmderror;
			}else{
				actUpon(2,tokens, false, false, -1);
			}
		}

		else if((tokens.at(0).compare("show")==0)){
			if((tokens.at(1).compare("tables")==0)&&tokens.size()==2)
				actUpon(3,tokens, true, false, -1);
			else if((tokens.at(1).compare("columns")==0)&&(tokens.at(2).compare("from")==0)&&tokens.size()==4&&isAlpha(tokens.at(3))){
				actUpon(10,tokens, true, true, 3);
			}
			else if(tokens.at(1).compare("databases")==0&&tokens.size()==2){
				actUpon(11,tokens, false, false, -1);
			}
			else if(tokens.at(1).compare("details")==0&&tokens.size()==2){
				actUpon(25,tokens, true, false, -1);
			}
			else{
				cmderror;
			}
		}

		else if((tokens.at(0).compare("insert")==0)&&(tokens.at(1).compare("into")==0)){
			if(!isAlpha(tokens.at(2))){
				cmderror;
				return 1;
			}else
				actUpon(5,tokens, true, true, 2);
		}

		else if((tokens.at(0).compare("select"))==0){
			actUpon(6,tokens, true, false, -1);//special case since we dont know exact table token
			return 2;
		}

		else if((tokens.at(0).compare("delete"))==0&&(tokens.at(1).compare("from"))==0&&isAlpha(tokens.at(2))){
			actUpon(7,tokens, true, true, 2);
		}

		else if((tokens.at(0).compare("update"))==0&&isAlpha(tokens.at(1))&&(tokens.at(2).compare("set"))==0){
			actUpon(8,tokens, true, true, 1);
		}

		else if(tokens.size()>3&&(tokens.at(0).compare("alter"))==0&&(tokens.at(1).compare("table"))==0&&isAlpha(tokens.at(2))){
			if(tokens.at(3).compare("rename")==0){
				if(tokens.size()==6&&tokens.at(4).compare("to")==0&&isAlpha(tokens.at(5))){
					actUpon(14,tokens, true, true, 2);
				}

				else if(tokens.size()==8&&tokens.at(4).compare("column")==0&&isAlpha(tokens.at(5))&&tokens.at(6).compare("to")==0&&isAlpha(tokens.at(7))){
					actUpon(15,tokens, true, true, 2);
				}

				else{
					cmderror;
					return 1;
				}
			}

			else if(tokens.at(3).compare("add")==0){
				actUpon(16,tokens, true, true, 2);
			}

			else if(tokens.size()==6&&tokens.at(3).compare("drop")==0&&tokens.at(4).compare("column")==0&&isAlpha(tokens.at(5))){
				actUpon(17,tokens, true, true, 2);
			}

			else{
				cmderror;
				return 1;
			}
		}

		else if((tokens.at(0).compare("drop"))==0&&tokens.size()<6){
			if(tokens.at(1).compare("database")==0)
				actUpon(12,tokens, false, false, -1);

			else if(tokens.at(1).compare("table")==0&&tokens.size()==3&&isAlpha(tokens.at(2))){
				actUpon(13,tokens, true, true, 2);
			}

			else{
				cmderror;
			}
		}

		else if(tokens.at(0).compare("import")==0&&tokens.at(1).compare("from")==0){
			loadFromFile(tokens.at(2));
		}

		else if(tokens.size()==2&&tokens.at(0).compare("close")==0&&tokens.at(1).compare("database")==0){
			actUpon(0,tokens, false, false, -1);
			dbs = NULL;
			cout << "Database Closed.\n";
		}

		else if(tokens.at(0).compare("@cache")==0){
			if(tokens.size()==4&&tokens.at(1).compare("set")==0&&tokens.at(2).compare("pagesize")==0&&isReal(tokens.at(3))){
				long pSize = atol(tokens.at(3).c_str());
				if(dbs==NULL){
					if(pSize>0){

						pageSize = pSize;
						cout << "PageSize set to "<<pageSize<<".\n";
					}else{
						cout << "Error in setting PageSize\n";
					}
				}else{
					cout << "Unable to change PageSize. Please close any open DB and try again.\n";
				}
			}

			else if(tokens.size()==3&&tokens.at(1).compare("show")==0&&tokens.at(2).compare("pagesize")==0){
				cout << "PageSize is "<<pageSize<<".\n";
			}
		}

		else{
			//final catch
			cmderror;
		}
	}catch(std::out_of_range &x){
		cmderror;
		return 1;
	}

	return 1;
}

void Parser::loadFromFile(string fileName){
	string ipFile = fileName;
	struct stat buf;
	string line;
	string query;
	char *log = new char[ipFile.size()+5];
	strcpy(log,ipFile.c_str());
	strcat(log,".log");
	if (stat(ipFile.c_str(), &buf) != -1){
		//exists
		freopen (log,"w",stdout);
		ifstream sqlfile (ipFile.c_str());
		long x = 0;
		if (sqlfile.is_open()){
			while ( !sqlfile.eof() ){
				query = "";
				do{
					getline (sqlfile,line);
					x++;
					//cout << x<<endl;
					query.append(" ");
					query.append(trim(line));
				}while(!sqlfile.eof()&&query[query.size()-1]!=';');
				if(query.size()>1){
					//cout << "=="<<query<<"==\n";
					parse(query);
				}
			}
			sqlfile.close();
			freopen("/dev/tty", "a", stdout);
			cout <<"Log written to "<< log<<endl;
		}else{
			cout << "Unable to open file."<<endl;
		}
	}else{
		cout << "File does not exists\n";
	}
	delete[] log;
	return;
}


Where* Parser:: prepareWhere(vector<string> tokens, vector<string> colNames, vector<int> colTypes, unsigned i){
	Where *where = new Where();
	WhereCond *wc;
	int op, toklen;
	char *value;
	int WhereConjuction;
	vector<string>::iterator itPos;

	do{
		if(i+4>tokens.size()){//atleast 4 to parse
			cmderror;
			return NULL;
		}

		if(tokens[i].compare("where")==0){
			WhereConjuction = 0;

		}else if(tokens[i].compare("and")==0){
			WhereConjuction = 1;
		}else if(tokens[i].compare("or")==0){
			WhereConjuction = 2;
		}else if(tokens[i].compare("not")==0){
			WhereConjuction = 3;
		}else{
			cmderror;
			return NULL;
		}

		i++;

		itPos = std::find(colNames.begin(),colNames.end(),tokens[i]);
		if(itPos == colNames.end()){
			cout << "Invalid Column Names."<<endl;
			return NULL;
		}
		i++;
		switch(tokens[i][0]){
		case '=':
			op = 0;
			break;
		case '>':
			if(tokens[i+1][0]=='='){
				op = 4;
				i++;
			}else
				op = 1;
			break;
		case '<':
			if(tokens[i+1][0]=='>'){
				op = 3;
				i++;
			}else if(tokens[i+1][0]=='='){
				op = 5;
				i++;
			}else
				op = 2;
			break;
		case '!':
			if(tokens[i+1][0]=='='){
				op = 3;
				i++;
				break;
			}
			//no break. because else is an error.
		default:
			cmderror;
			return NULL;
		}

		i++;

		switch(colTypes[itPos-colNames.begin()]){
		case 1:
			toklen = tokens[i].length()-1;
			if(tokens[i][0]!='\''||tokens[i][toklen]!='\''){
				cout << "Invalid Values."<<endl;
				return NULL;
			}else{
				tokens[i]=tokens[i].substr(1,toklen-1);
			}
			break;
		case 2:
			if(!isInt(tokens[i])){
				cout << "Invalid Values."<<endl;
				return NULL;
			}
			break;
		case 3:
			if(!isReal(tokens[i])){
				cout << "Invalid Values."<<endl;
				return NULL;
			}
			break;
		default:
			cmderror;
			return NULL;
		}
		value = new char[100];
		strcpy(value,tokens[i].data());
		wc = new WhereCond(itPos-colNames.begin(),colTypes[itPos-colNames.begin()],op,value);
		where->addCondition(WhereConjuction,wc);
		delete[] value;
		i++;

	}while(i!=tokens.size());

	return where;
}

void Parser::actUpon(int ch,vector<string> tokens, bool requiresOpenDB, bool requiresExistingTable, int tableTokenId){

	if(requiresOpenDB){
		if(dbs == NULL){
			cout << "No Database Loaded.\n";
			return;
		}
	}

	char tableName[100];
	Table *tbl = NULL;
	if(requiresExistingTable){
		strcpy(tableName,tokens[tableTokenId].c_str());
		tbl = new Table(tableName,dbs);
		if(tbl->getDirPageHeader()==NULL){
			cout << "Table does not exist."<<endl;
			delete tbl;
			return;
		}
	}

	char dbName[100];
	switch(ch){
	case 1: // create database
		strcpy(dbName,tokens[2].c_str());
		if(dbs != NULL)
			delete dbs;
		dbs = new DataBaseServer(dbName,true);
		if(dbs->isValid()){
			cout <<"'" << dbName<<"' created."<<endl;
		}else{
			cout <<"Error! Unable to create '"<<dbName<<
					"'. Another db with same Name exists."<<endl;
		}
		break;
	case 2: // load database
		//cout << endl << "Enter Database Name";
		strcpy(dbName,tokens[1].c_str());
		if(dbs != NULL)
			delete dbs;
		dbs = new DataBaseServer(dbName,false);
		if(dbs->isValid()==1){
			cout << dbName<<" Loaded"<<endl;
		}

		else if(dbs->isValid()==-1){
			delete dbs;
			dbs = NULL;

		}else{
			delete dbs;
			dbs = NULL;
			cout <<  "Error!"<< dbName<<" does not exist."<<endl;
		}
		break;
	case 3: // show tables
		dbs->showTables();
		break;
	case 4://creating table
	{
		int noOfattr = 0,i;
		char *attrName;
		vector<char*> aName;
		vector<int> maxSize, aType;

		strcpy(tableName,tokens[2].c_str());//tableName
		//skipping next (
		i = 4;
		while(1){
			//getting the column name
			if(isAlpha(tokens[i])){
				attrName = new char[100];
				strcpy(attrName,tokens[i].c_str());
				aName.push_back(attrName);
				i++;
			}else{
				cout << "Invalid Column Name."<<endl;
				return;
			}
			//getting type;
			string type = findType(tokens[i]);
			if(type.size()==1){
				aType.push_back(str2int(type));
				i++;
			}else{
				cmderror;
				return;
			}
			//getting maxsize if any
			if(tokens[i][0]=='('){
				i++;
				if(isInt(tokens[i])){
					//stmt.push_back(tokens[i++]);
					maxSize.push_back(str2int(tokens[i++]));
					if(tokens[i++][0]!=')'){
						cmderror;
						return;
					}
				}else{
					cmderror;
					return;
				}
			}else{
				if(type[0]=='1'){
					cmderror;
					return;
				}
				maxSize.push_back(50);
			}
			noOfattr++;
			if(tokens[i][0]==','){
				i++;
			}else if(tokens[i][0]==')'){
				break;//breaking while
			}else{
				cmderror;
				return;
			}
		}

		Table *tbl = new Table(tableName, aName, maxSize, aType,dbs);
		if(tbl->getDirPageHeader()==NULL){
			cout << "Cannot create Table. Duplicate Table Name."<<endl;
			for(unsigned i=0;i<aName.size();i++){
				delete[] aName[i];
			}
		}else{
			cout << "Table '"<<tableName<<"' created."<<endl;
		}
		delete tbl;
		break;
	}
	case 5://insert into
	{
		int noOfattr;
		unsigned i;
		vector<int> attrType;
		vector<string> attrName;
		vector<string> ipAttrNames;
		vector<char*> aName;
		vector<int> pos;
		int ipNum = 0;
		bool hasColumnNames = false;

		noOfattr = tbl->getNumOfAttr();
		//cout << "Total Attributes :"<<noOfattr;
		attrName = tbl->getAttributeNamesString();
		attrType = tbl->getAttributeTypes();
		i = 3;
		if(tokens[i][0]=='('){
			//start getting attribute names
			i++;
			do{
				if(isAlpha(tokens[i])){
					ipAttrNames.push_back(tokens[i]);
					i += 2;
				}else{
					delete tbl;
					cmderror;
					return;
				}
			}while(tokens[i-1][0]!=')'&&tokens[i-1][0]==',');
			hasColumnNames = true;

		}

		if(tokens[i].compare("values")==0){
			if(ipAttrNames.size()==0)
				ipAttrNames = attrName;
			ipNum = ipAttrNames.size();
		}else{
			//not both?. error it is.
			delete tbl;
			cmderror;
			return;
		}

		if(hasColumnNames){
			//the column names were given
			for(int k=0;k<ipNum;k++){
				for(int j=0;j<noOfattr;j++){
					if(ipAttrNames[k].compare(attrName[j])==0){
						pos.push_back(j);
						break;
					}
				}
			}
		}

		if(hasColumnNames&&(pos.size()!=ipAttrNames.size())){
			cout << "Invalid Column Names"<<endl;
			delete tbl;
			return;
		}

		RecordSet *rs = new RecordSet(attrType,noOfattr);
		Record *r;
		vector<string> values;
		int k;
		char *data;


		i++;//skip "values"
		do{//it comes here iff its value;
			if(tokens[i][0]!='('){
				delete tbl;
				delete rs;
				cmderror;
				return;
			}

			values.clear();

			k = 0;
			int toklen;

			do{
				i++;
				switch(attrType[(hasColumnNames)?pos[k]:k]){
				case 1:
					toklen = tokens[i].length()-1;
					if(tokens[i][0]!='\''||tokens[i][toklen]!='\''){
						cout << "Invalid Values."<<endl;
						delete tbl;
						delete rs;
						return;
					}else{
						tokens[i]=tokens[i].substr(1,toklen-1);
					}
					break;
				case 2:
					if(!isInt(tokens[i])){
						cout << "Invalid Values."<<endl;
						delete tbl;
						delete rs;
						return;
					}
					break;
				case 3:
					if(!isReal(tokens[i])){
						cout << "Invalid Values."<<endl;
						delete tbl;
						delete rs;
						return;
					}
					break;
				default:
					cmderror;
					return;
				}
				//correct format
				k++;
				values.push_back(tokens[i]);
				i++;
			}while(tokens[i][0]==',');

			if(k!=ipNum){
				delete tbl;
				delete rs;
				cmderror;
				return;
			}

			r = new Record();

			if(hasColumnNames){
				for(int j=0;j<noOfattr;j++){
					for(k =0;k<ipNum;k++){
						if(pos[k]==j){
							data = new char[values[k].size()+1];
							strcpy(data,values[k].c_str());
							r->addValue(data,attrType[j]);
							delete[] data;
							break;
						}
					}
					if(k==ipNum)//never went inside
						r->addValue("",attrType[j]);
				}
			}else{
				for(int j=0;j<noOfattr;j++){
					data = new char[values[j].size()+1];
					strcpy(data,values[j].c_str());
					r->addValue(data,attrType[j]);
					delete[] data;
				}
			}

			rs->addRecord(r);

			if(tokens[i++][0]!=')'){

				delete tbl;
				delete rs;
				cmderror;
				return;
			}

			if(i!=tokens.size()){
				if(tokens[i++][0]!=','){

					delete tbl;
					delete rs;
					cmderror;
					return;
				}
			}else
				break;

		}while(1);
		//rs->printAll(0,rs->getNumOfRecords());
		cout << tbl->insertTuples(rs) << " records inserted.\n";

		delete rs;
		delete tbl;
		break;
	}
	case 6: // select
	{
		vector<string>::iterator itPos;
		unsigned i;
		vector<string> colNames;
		vector<int> colTypes;
		vector<int> pos;

		Select *select = new Select();
		Where *where;

		itPos = std::find(tokens.begin(),tokens.end(),"from");
		if(itPos ==tokens.end()){
			//NO from.
			cmderror;
			return;
		}
		//valid from:
		i = itPos - tokens.begin();

		i++;//go to table name

		if(!isAlpha(tokens.at(i))){
			cout << "Invalid table name.\n";
			delete select;
			return;
		}

		strcpy(tableName,tokens[i].c_str());

		Table *tbl = new Table(tableName,dbs);
		if(tbl->getDirPageHeader()==NULL){
			cout <<"Table does'nt exist."<<endl;
			delete select;
			delete tbl;
			return;
		}

		colNames = tbl->getAttributeNamesString();
		colTypes = tbl->getAttributeTypes();

		i++;
		if(i!=tokens.size()){//not the end
			if(tokens[i].compare("where")==0){
				//Where present;
				where = prepareWhere(tokens,colNames,colTypes,i);
				if(where == NULL){
					return;
				}
			}else{
				delete select;
				delete tbl;
				return;
				cmderror;
				return;
			}
		}else{
			where = new Where();
		}

		//where->toString();

		//now for select;
		if(tokens[1][0]=='*'){
			if(tokens[2].compare("from")!=0){
				//error syntax
				delete select;
				delete tbl;
				delete where;
				cmderror;
				return;
			}
		}

		else if(tokens[1].compare("from")!=0){
			i = 0;
			int colPos = -1;

			do{
				colPos = tbl->getColumnPos(tokens[++i]);
				//itPos = std::find(colNames.begin(),colNames.end(),tokens[++i]);
				if(colPos == -1){
					cout << "Invalid Column Names"<<endl;
					delete select;
					delete tbl;
					delete where;
					return;
				}else{
					pos.push_back(colPos);
				}

			}while(tokens[++i][0]==',');
			if(tokens[i].compare("from")!=0){
				delete select;
				delete tbl;
				delete where;
				cmderror;
				return;
			}

			delete select;//deleting the previous select.
			select = new Select(pos,pos.size(),colTypes,colTypes.size());

		}

		else{
			delete select;
			delete tbl;
			delete where;
			cmderror;
			return;
		}

		RecordSet *rs;

		string choice;

		do{
			rs = tbl->selectTuples(select,where);

			rs->printAll(0,rs->getNumOfRecords());

			if(select->isThereMore()){
				cout << "Get More?(Y/N) : ";
				cin >> choice;
			}

		}while(select->isThereMore()&&(choice.at(0)=='y'||choice.at(0)=='Y'));

		delete rs;
		delete tbl;
		delete select;
		delete where;
		break;
	}
	case 7: /*delete from x where */
	{
		Where *where;

		vector<string> colNames;
		vector<int> colTypes;

		colNames = tbl->getAttributeNamesString();
		colTypes = tbl->getAttributeTypes();

		if(tokens.size()>3){//not the end
			if(tokens[3].compare("where")==0){
				//Where present;
				where = prepareWhere(tokens,colNames,colTypes,3);
				//where->toString();
				if(where == NULL){
					return;
				}
			}else{
				cmderror;
				return;
			}
		}else{
			where = new Where();
		}

		cout << tbl->deleteTuples(where) <<" rows deleted."<<endl;

		delete where;
		delete tbl;
		break;
	}
	case 8://modify
	{
		vector<string> colNames;
		vector<int> colTypes;
		unsigned i = 3;//update tblname set
		int pos = -1;

		colNames = tbl->getAttributeNamesString();
		colTypes = tbl->getAttributeTypes();

		int toklen;
		char *value;

		Where *where;
		Modify *modify = new Modify(colTypes,colTypes.size());

		do{
			pos = tbl->getColumnPos(tokens[i]);

			if(pos==-1){
				cout << "Invalid Column Names."<<endl;
			}else{
				i++;
				if(tokens[i][0]!='='){
					cmderror;
					return;
				}

				i++;
				switch(colTypes[pos]){
				case 1:
					toklen = tokens[i].length()-1;
					if(tokens[i][0]!='\''||tokens[i][toklen]!='\''){
						cout << "Invalid Values."<<endl;
						return;
					}else{
						tokens[i]=tokens[i].substr(1,toklen-1);
					}
					break;
				case 2:
					if(!isInt(tokens[i])){
						cout << "Invalid Values."<<endl;
						return;
					}
					break;
				case 3:
					if(!isReal(tokens[i])){
						cout << "Invalid Values."<<endl;
						return;
					}
					break;
				default:
					cmderror;
					return;
				}
				value = new char[100];
				strcpy(value,tokens[i].data());
				modify->addUpdatedValues(value,pos);
				delete[] value;
			}
			if(tokens.size()==++i){
				break;
			}

			else if(tokens[i][0]==','){
				i++;
			}

			else if(tokens[i].compare("where")==0){

				break;
			}

			else{
				cmderror;
				return;
			}

		}while(1);
		if(tokens.size()!=i)
			where = prepareWhere(tokens,colNames,colTypes,i);
		else{
			where = new Where();
		}


		cout << tbl->updateTuples(where,modify) << " records updated."<<endl;

		delete where;
		delete modify;
		delete tbl;
		break;
	}
	case 9://create index
		//Not supported yet
		break;

	case 10: //show columns from tables
	{
		int noOfattr;
		vector<int> attrType;
		vector<char*> attrName;

		noOfattr = tbl->getNumOfAttr();
		//cout << "Total Attributes :"<<noOfattr;
		attrName = tbl->getAttributeNames();
		attrType = tbl->getAttributeTypes();
		cout << "'Columns'\t"<<"'Type'"<<endl;
		for(int i=0;i<noOfattr;i++){
			cout << attrName[i]<<"\t";
			switch(attrType[i]){
			case 1:
				cout << "VARCHAR";
				break;
			case 2:
				cout << "INT";
				break;
			case 3:
				cout << "REAL";
				break;
			default:
				cout << "ERROR";
				break;
			}
			cout << endl;
		}
		break;
	}

	case 11://Show databases
		DIR *dir;
		struct dirent *ent;
		char dirpath[FILENAME_MAX];
		DataBaseServer::getDataDirectory(dirpath);
		if ((dir = opendir (dirpath)) != NULL) {
			/* print all the files and directories within directory */
			string db;
			int n = 0;
			while ((ent = readdir (dir)) != NULL) {
				db = ent->d_name;
				//cout << db << " "<<db.find(".db",3)<<" "<<db.size()-2<<endl;
				if(db.size()>3&&(db.substr(db.size()-3)).compare(".db")==0){
					cout << db.substr(0,db.size()-3) << endl;
					n++;
				}
			}
			if(n==0){
				cout << "No databases found.\n";
			}
			closedir (dir);
		} else {
			/* could not open directory */
			perror ("");
			return;
		}
		break;

	case 12:/*drop database*/
		if(isAlpha(tokens[2])){
			char *filepath = new char[300];
			char dirpath[FILENAME_MAX];
			DataBaseServer::getDataDirectory(dirpath);
			strcpy(filepath,dirpath);
			strcat(filepath,tokens[2].c_str());
			strcat(filepath,".db");
			ifstream file(filepath);
			if(file.is_open()){
				file.close();
				if(remove(filepath)==0){
					cout <<"'"<< tokens[2]<<"' dropped."<<endl;
					//cout << dbs->dbName<<" "<<tokens[2].c_str()<<"\n";
					if(dbs!=NULL&&tokens[2].compare(dbs->dbName)==0){
						delete dbs;
						dbs = NULL;
					}
				}else{
					cout << "Error deleting "<<tokens[2]<<endl;
				}
			}else{
				cout << "No such database exists."<<endl;
			}
			delete [] filepath;
		}
		break;

	case 13://drop table
		if(tbl->deleteTable()==1)
			cout << "Table '"<<tokens[2]<<"' dropped."<<endl;
		else
			cout << "Error in deleting table."<<endl;
		break;

	case 14:/*alter table rename*/
		if(tbl->alterTable(tokens[5])==1){
			cout <<"Table '"<< tableName<<"' renamed to '"<< tokens[5]<<"'.\n";
		}else{
			cout << "Error in renaming the table.\n";
		}
		delete tbl;
		break;

	case 15:/*alter table rename column*/
		if(tbl->alterTable(tokens[5],tokens[7])==1){
			cout <<"Table '"<< tableName<<"' : column '"<<tokens[5]<<"' renamed to '"<< tokens[7]<<"'.\n";
		}else{
			cout << "Error in renaming the table.\n";
		}
		delete tbl;
		break;

	case 16:/*alter table add column*/
	{
		vector<int> type;
		vector<int> maxSize;
		vector<string> colName;

		//alter table x add column cname varchar(20);
		if(tokens[5][0]=='('){
			cmderror;
			return;
		}else{
			if(isAlpha(tokens[5])){
				colName.push_back(tokens[5]);
				type.push_back(atoi(findType(tokens[6]).c_str()));
				if(tokens.size()!=7&&tokens[7][0]=='('&&tokens[9][0]==')'){
					maxSize.push_back(atoi(tokens[8].c_str()));
				}else{
					maxSize.push_back(-1);
				}
			}

			else{
				cmderror;
				return;
			}
		}


		if(tbl->alterTable(colName,type,maxSize)==1){
			cout <<"Column added.\n";
		}else{
			cout << "Error in adding the column.\n";
		}

		delete tbl;
		break;
	}
	case 17:/*alter table drop column*/
		if(tbl->alterTableDropColumn(tokens[5])==1){
			cout <<"Column dropped.\n";
		}else{
			cout << "Error in dropping the Column.\n";
		}
		delete tbl;
		break;

	case 18://drop index;
		//not supported yet
        break;

	case 25://show DB details
		dbs->showDBDetails();
		break;

	case 26://delete database
		delete dbs;
		break;

	default:
		cmderror;
		break;
	}
}

} /* namespace datamodels */
