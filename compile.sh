#!/bin/bash
rm src/*~
mkdir -p build
cd build
	cmake ../
	make
cd ..
mkdir -p bin
cp -b ./build/src/Jabba ./bin/Jabba
cd src
#doxygen ../Profiling/Doxyfile > ../Profiling/doxygen_output
#pmccabe -vt *.hpp *.cpp > ../Profiling/pmccabe_output
