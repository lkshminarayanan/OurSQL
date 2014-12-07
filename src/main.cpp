#include "Parser.h"

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
