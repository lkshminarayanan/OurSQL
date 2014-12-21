OurSQL
======

An experimental attempt at creating a simple, standalone, serverless Database Management System with an ultimate goal of learning the internals. Supports simple, basic queries. SQL statements have the same syntax as MySQL's. Runs only on linux machines.

###Installation
1. It can be installed by running the install.sh shell script (or)
2. cmake . & make install

The executable binary will be installed inside OurSQL/bin

###Configuring
The executable takes only a data-directory location as a command line argument, which also is optional. By default, OurSQL creates a data directory in the current working directory.

###Supported Queries
The following queries are supported in OurSQL. All SQL commands are case insensitive.

####Database Operations

1. CREATE DATABASE *dbname*;
2. SHOW DATABASES;
3. USE *dbname*;
4. CLOSE DATABASE;
5. DROP DATABASE *dbname*;

####Table Operations

6. CREATE TABLE *tblname* (*col1* *col1-type*[(*col1-width*)] [, *col2* *col2-type*[(*col2-width*)] ,...]);
7. INSERT INTO *tblname* [(*col1*, *col2*,...)] VALUES (*val1*[, *val2* ,...]);
8. SELECT \*|*col1*[,*col2* ,...] FROM *tblname* [WHERE *cond*];
9. UPDATE *tblname* SET *col*=*val* [WHERE *cond*];
10. DELETE FROM *tblname* [WHERE *cond*];
11. SHOW TABLES;
12. SHOW COLUMNS FROM *tblname*;
13. ALTER TABLE *tblname* RENAME TO *new-tbl-name*;
14. ALTER TABLE *tblname* RENAME COLUMN *col-name* TO *new-col-name*;
15. ALTER TABLE *tblname* ADD COLUMN *new-col* *new-coltype*;
16. ALTER TABLE *tblname* DROP COLUMN *col-name*;
17. DROP TABLE *tblname*;

####Processing SQL Batch File
18. IMPORT FROM *complete-sql-file-path*;
