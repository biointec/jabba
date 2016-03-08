add_executable(jabba GraphChain.cpp IntraNodeChain.cpp InterNodeChain.cpp Graph.cpp SeedFinder.cpp AlignedRead.cpp Settings.cpp Nucleotide.cpp TString.cpp Alignment.cpp mummer/qsufsort.c mummer/sparseSA.cpp ReadCorrection.cpp ReadCorrectionHandler.cpp library.cpp util.cpp)
target_link_libraries(jabba readfile pthread)
add_subdirectory(readfile)
