#!/bin/bash

#if build is not there..
mkdir -p build
cd build

cmake ..
make install
