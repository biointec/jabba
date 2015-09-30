#!/bin/bash
rm src/*~
mkdir -p build
cd build
	cmake ../
	make
cd ..
mkdir -p bin
cp -b ./build/src/Jabba ./bin/Jabba
