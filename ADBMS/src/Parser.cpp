/*
 * Parser.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: latchu
 */

#include "Parser.h"


namespace dbEngine {

Parser::Parser() {
	dbs = NULL;
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

	//std::transform(stmt.begin(), stmt.end(), stmt.begin(), ::tolower);

	if((stmt.at(stmt.size()-1)!=';')||stmt.size()<2){
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
					tokens.push_back(item.substr(prev, pos-prev));

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
				tokens.push_back(item.substr(prev, std::string::npos));
			}

		}
	}

	int n = tokens.size();
	for(int i = 0;i<n;i++){
		cout << tokens[i] << endl;
	}


	if(tokens[0].compare("exit")==0||(tokens[0].compare("goodbye")==0)){
		cout << endl<< "Bye!";
		actUpon(0,tokens);
		return 0;
	}

	//return 1;


	else if(tokens[0].compare("create")==0){
		if(tokens[1].compare("database")==0){
			if(isAlpha(tokens[2])&&tokens.size()==3)
				actUpon(1,tokens);
			else{
				cmderror;
				return 1;
			}
		}

		else if(tokens[1].compare("table")==0){
			//create table tblname (col1 col1type[],col2 col2type[],..);
			if(!isAlpha(tokens[2])){
				cmderror;
				return 1;
			}else
				actUpon(4,tokens);

		}
	}


	else if(tokens[0].compare("use")==0){
		if(tokens.size()>2){
			cmderror;
		}else{
			actUpon(2,tokens);
		}
	}

	else if((tokens[0].compare("show")==0)){
		if((tokens[1].compare("tables")==0)&&tokens.size()==2)
			actUpon(3,tokens);
		else if((tokens[1].compare("columns")==0)&&(tokens[2].compare("from")==0)&&tokens.size()==4&&isAlpha(tokens[3])){
			actUpon(10,tokens);
		}
		else if(tokens[1].compare("databases")==0&&tokens.size()==2){
			actUpon(11,tokens);
		}
		else{
			cmderror;
		}
	}

	else if((tokens[0].compare("insert")==0)&&(tokens[1].compare("into")==0)){
		if(!isAlpha(tokens[2])){
			cmderror;
			return 1;
		}else
			actUpon(5,tokens);
	}

	else if((tokens[0].compare("select"))==0){
		actUpon(6,tokens);
	}

	else if((tokens[0].compare("delete"))==0&&(tokens[1].compare("from"))==0&&isAlpha(tokens[2])){
		actUpon(7,tokens);
	}

	else if((tokens[0].compare("update"))==0&&isAlpha(tokens[1])&&(tokens[2].compare("set"))==0){
		actUpon(8,tokens);
	}

	else if((tokens[0].compare("drop"))==0&&tokens.size()==3){
		if(tokens[1].compare("database")==0)
			actUpon(12,tokens);
		else{
			cmderror;
		}
	}

	else{
		//final catch
		cmderror;
	}

	return 1;
}

Where* Parser:: prepareWhere(vector<string> tokens, vector<string> colNames, vector<int> colTypes, int i){
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
		i++;

	}while(i!=tokens.size());

	return where;
}

void Parser::actUpon(int ch,vector<string> tokens){
	char dbName[100];
	Select *select = new Select();
	Where *where = new Where();

	/*cout<<endl<<"1.Create Database";
	cout<<endl<<"2.Load Database";
	cout<<endl<<"3.Show Tables";
	cout<<endl<<"4.Create Table";
	cout<<endl<<"5.Insert to table";
	cout<<endl<<"6.Select *";
	cout<<endl<<"7.Select colnames";
	cout<<endl<<"";where
	cout<<endl<<"0.Exit";
	cin>>ch;*/
	switch(ch){
	case 0:
		if(dbs != NULL)
			delete dbs;
		break;
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
		if(dbs->isValid()){
			cout << dbName<<" Loaded"<<endl;
		}else{
			cout <<  "Error!"<< dbName<<" does not exist."<<endl;
		}
		break;
	case 3: // show tables
		if(dbs == NULL){
			cout << "No Database Loaded"<<endl;
		}else{
			dbs->showTables();
		}
		break;
	case 4://creating table
		if(dbs == NULL){
			cout << "No Database Loaded"<<endl;
		}else{

			int noOfattr = 0,i;
			char *attrName,tableName[100];
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


			//create table x(col1 int, col2 int(12),col3 varchar(10),col5 varchar(15),col6 real, col6 float(10));



			Table *tbl = new Table(tableName, aName, maxSize, aType,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout << "Cannot create Table. Duplicate Table Name."<<endl;
			}else{
				cout << "Table '"<<tableName<<"' created."<<endl;
				delete tbl;
			}
		}
		break;
	case 5://insert into
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			int noOfattr,i;
			vector<int> attrType;
			vector<string> attrName;
			vector<string> ipAttrNames;
			char tableName[100];
			vector<char*> aName;
			vector<char*> values;
			vector<int> pos;
			int ipNum = 0;
			bool hasColumnNames = false;

			strcpy(tableName,tokens[2].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout << "Table does not exist."<<endl;
			}else{
				noOfattr = tbl->getNumOfAttr();
				//cout << "Total Attributes :"<<noOfattr;
				attrName = tbl->getAttributeNamesString();
				attrType = tbl->getAttributeTypes();
				i = 3;
				if(tokens[i][0]=='('){

					//start getting attribute names
					cout << "getting names";
					i++;
					do{
						if(isAlpha(tokens[i])){
							ipAttrNames.push_back(tokens[i]);
							i += 2;
						}else{
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

				for(int j=0;j<ipNum;j++)
					cout << ipAttrNames[j] <<endl;

				for(int j=0;j<pos.size();j++)
					cout << pos[j] <<endl;

				if(hasColumnNames&&(pos.size()!=ipAttrNames.size())){
					cout << "Invalid Column Names"<<endl;
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
						//correct format
						k++;
						values.push_back(tokens[i]);
						i++;
					}while(tokens[i][0]==',');

					if(k!=ipNum){
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
						}
					}

					rs->addRecord(r);

					if(tokens[i++][0]!=')'){
						cmderror;
						return;
					}

					if(i!=tokens.size()){
						if(tokens[i++][0]!=','){
							cmderror;
							return;
						}
					}else
						break;

				}while(1);
				//rs->printAll(0,rs->getNumOfRecords());
				cout << tbl->insertTuples(rs) << " records inserted."<<endl;
				delete tbl;
			}
		}

		break;
	case 6: // select
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;

		}else{
			//cout << "Where "<< (std::find(tokens.begin(),tokens.end(),"where"))-tokens.begin();
			vector<string>::iterator itPos;
			char *tableName = new char[100];
			int i;
			vector<string> colNames;
			vector<int> colTypes;
			vector<int> pos;

			itPos = std::find(tokens.begin(),tokens.end(),"from");
			if(itPos ==tokens.end()){
				//NO from.
				cmderror;
				return;
			}
			//valid from:
			i = itPos - tokens.begin();


			strcpy(tableName,tokens[++i].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout <<"Table does'nt exist."<<endl;
				return;
			}

			//if table exists

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
					cmderror;
					return;
				}
			}

			//where->toString();

			//now for select;
			if(tokens[1][0]=='*'){
				if(tokens[2].compare("from")!=0){
					//error syntax
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
						return;
					}else{
						pos.push_back(colPos);
					}

				}while(tokens[++i][0]==',');
				if(tokens[i].compare("from")!=0){
					cmderror;
					return;
				}

				select = new Select(pos,pos.size(),colTypes,colTypes.size());

			}

			else{
				cmderror;
				return;
			}

			RecordSet *rs = tbl->selectTuples(select,where);
			rs->printAll(0,rs->getNumOfRecords());



		}
		break;
	case 7: /*delete from x where */
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			char *tableName = new char[100];
			strcpy(tableName,tokens[2].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout <<"Table does'nt exist."<<endl;
			}else{

				Where *where = new Where();

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
				}

				cout << tbl->deleteTuples(where,colTypes,colTypes.size()) <<" rows deleted."<<endl;

			}
		}
		break;
	case 8://modify
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			char tableName[100];
			strcpy(tableName,tokens[1].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout <<"Table does'nt exist."<<endl;
			}else{



				vector<string> colNames;
				vector<int> colTypes;
				int i = 3;//update tblname set
				int pos = -1;

				colNames = tbl->getAttributeNamesString();
				colTypes = tbl->getAttributeTypes();

				int toklen;
				char *value;

				Where *where = new Where();
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
					}
					if(tokens[++i][0]==','){
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
				where = prepareWhere(tokens,colNames,colTypes,i);


				cout << tbl->updateTuples(where,modify) << " records updated."<<endl;
			}
		}
		break;
	case 10:
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			int noOfattr;
			vector<int> attrType;
			vector<char*> attrName;
			char tableName[100];

			strcpy(tableName,tokens[3].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout << "Table does not exist."<<endl;
			}else{
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
			}
		}
		break;
	case 11:
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (dirpath)) != NULL) {
			/* print all the files and directories within directory */
			string db;
			while ((ent = readdir (dir)) != NULL) {
				db = ent->d_name;
				//cout << db << " "<<db.find(".db",3)<<" "<<db.size()-2<<endl;
				if(db.size()>3&&(db.substr(db.size()-3)).compare(".db")==0)
					cout << db.substr(0,db.size()-3) << endl;
			}
			closedir (dir);
		} else {
			/* could not open directory */
			perror ("");
			return;
		}
		break;

	case 12:
		if(isAlpha(tokens[2])){
			char *filepath = new char[300];
			strcpy(filepath,dirpath);
			strcat(filepath,tokens[2].c_str());
			strcat(filepath,".db");
			ifstream file(filepath);
			if(file.is_open()){
				file.close();
				if(remove(filepath)==0){
					cout <<"'"<< tokens[2]<<"' dropped."<<endl;
				}else{
					cout << "Error deleting "<<tokens[2]<<endl;
				}
			}else{
				cout << "No such database exists."<<endl;
			}
		}
		break;


	default:
		cmderror;
		break;
	}
}


} /* namespace datamodels */

using namespace dbEngine;

int main(){
	string stmt;
	string substmt;
	Parser p;
	do{
		cout << "OurSQL$: ";
		getline(cin, stmt);

		//shortcuts for testing.
		if(stmt[0]=='.'){
			if(stmt[1]=='.'){
				stmt = "use iiitb;";
			}
			else if(stmt[1]=='a'){
				stmt = "show tables;";
			}

			else if(stmt[1]=='s'){
				string tbl = stmt.substr(2);
				stmt = "select * from ";
				stmt.append(tbl);
				stmt.append(";");
			}

			else if(stmt[1]=='x'){
				stmt = "exit;";
			}

			else if(stmt[1]=='t'){
				stmt = "update sam set age=24, mark=98 where age=23;";
			}
		}//end shortcuts

		while(stmt[stmt.size()-1]!=';'){
			cout << "     >";
			getline(cin,substmt);
			if(substmt.size()>0){
				if(stmt.size()>0) stmt.append(" ");
				stmt.append(substmt);
			}
		}

	}while(p.parse(stmt)!=0);
	return 0;
}


/**
 * create_tree create
 * insert.h insert
 * find_key.h findKey - lists all duplicates.
 * modify.h - modify
 * range_search.h -
 * delete key - full key delete
 * delete key rid - rid delete
 *
 *
 *
 * */
