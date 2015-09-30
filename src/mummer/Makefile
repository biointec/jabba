FLAGS = -O3 -DNDEBUG
SRC = calling_essaMEM.cpp mummer.cpp qsufsort.c sparseSA.cpp fasta.cpp

all: mummer calling_essaMEM

calling_essaMEM: calling_essaMEM.o qsufsort.o sparseSA.o fasta.o
	g++ $(FLAGS) $^ -o $@ -lpthread

mummer: mummer.o qsufsort.o sparseSA.o fasta.o
	g++ $(FLAGS) $^ -o $@ -lpthread

.cpp.o:
	g++ $(FLAGS) -Wall -c $<

.c.o:
	gcc $(FLAGS) -Wall -c $<

# .PHONY assures clean is exected even if there is a file "./clean" in
# the directory. The same for doc.
.PHONY: clean doc
doc: 
	doxygen
clean: 
	rm -f *.o *~ .depend mummer

# Create all the dependencies between the source files. 
.depend:
	g++ -MM $(SRC) > .depend

# The - prevents make from complaining about a missing .depend
-include .depend
