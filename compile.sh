#!/bin/bash
mkdir -p build
cd build
	cmake ..
	make -j
cd ..
mkdir -p bin
cp ./build/src/jabba ./bin/jabba
#cd src
#doxygen ../Profiling/Doxyfile > ../Profiling/doxygen_output
#pmccabe -vt *.hpp *.cpp > ../Profiling/pmccabe_output

git submodule update --init
cd brownie
	mkdir -p build
	cd build
		cmake -DMAXKMERLENGTH=75 ..
		make -j
	cd ..
cd ..
cp ./brownie/build/src/brownie ./bin/brownie

cd karect
	make -j
cd ..
cp ./karect/karect ./bin/karect

