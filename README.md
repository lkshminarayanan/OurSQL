OurSQL
======

An experimental attempt at creating a simple, standalone, serverless Database Management System with an ultimate goal of learning the internals. Supports simple, basic queries. SQL statements have the same syntax as MySQL's. Runs only on linux machines.

###Installation
1. It can be installed by running the install.sh shell script (or)
2. cmake . & make install

The executable binary will be installed inside OurSQL/bin

###Configuring
The executable takes only a data-directory location as a command line argument, which also is optional. By default, OurSQL creates a data directory in the current working directory.
