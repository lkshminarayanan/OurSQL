#include "Parser.h"
#include <errno.h>

using namespace dbEngine;

bool setDataDirectory(int argc, char *argv[]){
	//determine the data directory path
	char dirpath[FILENAME_MAX];
	if(argc == 2){
		//user provided dd
		strcpy(dirpath, argv[1]);
		strcat(dirpath, "/");
		DataBaseServer::setDataDirectory(dirpath);
	} else {
		//use cwd+"oursql.datadir" as dd
		if (!getcwd(dirpath, sizeof(dirpath))){
			error("Error finding out the current working directory");
			return false;
		}
		strcat(dirpath, "/oursql.datadir/");
		DataBaseServer::setDataDirectory(dirpath);
	}
	//create the directory if it doesnot exist
	if(mkdir(dirpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
		//error creating the folder.
		if(errno != EEXIST){
			error("Error creating the Data Directory : " <<
				   errno << " - " << strerror(errno));
			return false;
		}
	}
	cout << "Loaded Data Directory from : " << dirpath << "\n";
	return true;
}

int main(int argc, char *argv[]){
	string stmt;
	string substmt;
	Parser p;
	if(!setDataDirectory(argc, argv)){
		exit(-1);
	}
	do{
		cout << "OurSQL $ ";
		getline(cin, stmt);
		//shortcuts for testing.
		if(stmt[0]=='.'){
			if(stmt[1]=='.'){
				stmt = "use test;";
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
