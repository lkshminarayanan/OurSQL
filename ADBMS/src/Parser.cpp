/*
 * Parser.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: latchu
 */

#include "Parser.h"


namespace dbEngine {

Parser::Parser() {
	// TODO Auto-generated constructor stub
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
		while ((pos = item.find_first_of(" ,()<=>", prev)) != std::string::npos)
		{
			if (pos > prev)
				tokens.push_back(item.substr(prev, pos-prev));
			prev = pos+1;
			if(item[pos]!=' ')
				tokens.push_back(item.substr(pos, 1));
		}
		if (prev < item.length())
			tokens.push_back(item.substr(prev, std::string::npos));
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

	else{
		//final catch
		cmderror;
	}

	return 1;
}

void Parser::actUpon(int ch,vector<string> tokens){
	char dbName[100];

	/*cout<<endl<<"1.Create Database";
	cout<<endl<<"2.Load Database";
	cout<<endl<<"3.Show Tables";
	cout<<endl<<"4.Create Table";
	cout<<endl<<"5.Insert to table";
	cout<<endl<<"6.Select *";
	cout<<endl<<"7.Select colnames";
	cout<<endl<<"";
	cout<<endl<<"0.Exit";
	cin>>ch;*/
	switch(ch){
	case 0:
		if(dbs != NULL)
			delete dbs;
		break;
	case 1:
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
	case 2:
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
	case 3:
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

				//TODO:find the order of the ipNames;

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

				do{//it comes here iff its value;
					if(tokens[++i][0]!='('){
						cmderror;
						return;
					}

					values.clear();

					k = 0;

					do{
						i++;
						switch(attrType[(hasColumnNames)?pos[k]:k]){
						case 1:
							if(!isAlpha(tokens[i])){
								cout << "Invalid Valuesa."<<tokens[i]<<endl;
								return;
							}
							break;
						case 2:
							if(!isInt(tokens[i])){
								cout << "Invalid Value : "<<tokens[i]<<endl;
								return;
							}
							break;
						case 3:
							if(!isReal(tokens[i])){
								cout << "Invalid Valuesr."<<tokens[i]<<endl;
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
						if(tokens[i++][0]!=','||tokens[i].compare("values")!=0){
							cmderror;
							return;
						}
					}else
						break;

				}while(1);
				rs->printAll(0,rs->getNumOfRecords());
				tbl->insertTuples(rs);
				delete tbl;
			}
		}

		break;
	case 6:
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			char *tableName = new char[100];
			strcpy(tableName,tokens[3].c_str());
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout <<"Table does'nt exist."<<endl;
			}else{
				RecordSet *rs = tbl->selectTuples();
				rs->printAll(0,rs->getNumOfRecords());
			}
		}
		break;
	case 7:
		if(dbs == NULL){
			cout <<"No Database Loaded."<<endl;
		}else{
			char tableName[100];
			cout <<endl<<"TableName? :";
			cin >> tableName;
			Table *tbl = new Table(tableName,dbs);
			if(tbl->getDirPageHeader()==NULL){
				cout << endl<<"Table doesnot exist.";
			}else{
				int numOfCol;
				vector<char*> attrName = tbl->getAttributeNames();
				vector<char*> selAttrName;
				char *colName;
				int tableTotalCol = tbl->getNumOfAttr();
				cout << endl << "No. of columns to select :";
				cin>>numOfCol;
				cout <<endl<<"Choose the column [";
				for(int i=0;i<tableTotalCol;i++){
					cout <<" '"<<attrName[i]<<"' ";
				}
				cout << "]";
				for(int i=0;i<numOfCol;i++){
					colName = new char[100];
					cin >> colName;
					selAttrName.push_back(colName);
				}
				RecordSet *rs = tbl->selectTuples(1,selAttrName);
				rs->printAll(0,100);
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

	default:
		cmderror;
		break;
	}
}


} /* namespace datamodels */

using namespace dbEngine;

int main(){
	string stmt;
	Parser p;
	do{
		cout << "OurSQL$: ";
		getline(cin, stmt);
	}while(p.parse(stmt)!=0);
	return 0;
}
