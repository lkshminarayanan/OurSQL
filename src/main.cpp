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

int main(int argc, char* argv[]){
	string stmt;
	string substmt;
	Parser p;
	timeval startTime, endTime;
	double elapsedTime;
	char* buf = NULL;
	int response = 0;

	if(!setDataDirectory(argc, argv)){
		exit(-1);
	}

	do{
		stmt = "";
		buf = readline("OurSQL$: ");
		stmt.append(buf);
		stmt = trim(stmt);

		free (buf);

		//shortcuts for testing.
		if(stmt[0]=='.'){
			if(stmt[1]=='.'){
				stmt = "use iiitb;";
			}
			else if(stmt[1]=='a'){
				stmt = "show details;";
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
				stmt = "import from test_.sql;";
				//stmt = "import from create_member.sql;";
			}
		}//end shortcuts

		while(stmt[stmt.size()-1]!=';'){
			buf = readline("       >");
			if(buf!=NULL&&strlen(buf)>0){
				if(stmt.size()>0) stmt.append(" ");
				stmt.append(buf);
			}
			free(buf);
		}

		add_history(stmt.c_str());
		gettimeofday(&startTime,NULL);
		response = p.parse(stmt);
		gettimeofday(&endTime,NULL);

		if(response == 2){
			elapsedTime = (endTime.tv_sec - startTime.tv_sec);      // sec
			elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000000.0;   // us to s
			cout << "Query executed in "<<elapsedTime<<" seconds."<<endl;
		}

	}while(response!=0);

	clear_history();
	return 0;
}
