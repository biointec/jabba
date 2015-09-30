Jabba: Hybrid Error Correction for Long Sequencing Reads using Maximal Exact Matches
=====

Jabba takes as input a de Bruijn graph and a set of sequences:
- linear paths in the de Bruijn should be concatenated. The graph should be in the following file format, where each node spans 4 lines, as follows:  

>\<meta information\>  
>\<sequence\>  
>left arcs	\<in edges represented by node number of origin, separated by tabs\>  
>right arcs	\<out edges represented by node number of target, separated by tabs\>  

- the set of sequences will be corrected (e.g. PacBio reads), the corrections will be written to a file Jabba-\<input filename\>.fasta.

The output is a file with corrections of the long reads.


de Bruijn graph
===============
To build a de Bruijn graph from sequencing reads, one can use [brownie](https://github.com/jfostier/brownie), or any other tool.

brownie will concatenate linear nodes and can output the graph in the desired graph format.  
To build a graph with brownie from a fastq file short_reads.fastq containing short reads:  
>./brownie brownie_data 31 -short -fastq short_reads.fastq --graph  

To build a graph with brownie from a fasta file genome.fasta containing a reference genome (in this case the graph is not corrected):  
>./brownie brownie_data 31 -short -fasta genome.fastq --perfectgraph  

In both cases the graph file DBGraph.txt will be put in the current directory.

Installation
============
At the moment Jabba is available for Linux.
Jabba can be compiled as follows:
>mkdir -p build  
>cd build  
>cmake ../  
>make
>cd ..  
>mkdir -p bin  
>cp -b ./build/src/Jabba ./bin/Jabba  

Usage
=====
>Jabba [options] [file_options] file1 [[file_options] file2]...  
[options]  
  -h	--help		display help page  
  -i	--info		display information page  
 [options arg]  
  -l	--length	minimal seed size [default = 20]  
  -k	--dbgk		de Bruijn graph k-mer size  
  -e	--essak		sparseness factor of the enhance suffix array [default = 1]  
  -t	--threads	number of threads [default = available cores]  
  -p	--passes	maximal number of passes per read [default = 2]  
 [file_options file_name]  
  -o	--output	output directory [default = Jabba_output]  
  -fastq		fastq input files  
  -fasta		fasta input files  
  -g	--graph		graph input file [default = DBGraph.txt]  

examples:  
>./Jabba --dbgk 31 --graph DBGraph.txt -fastq reads.fastq  
./Jabba -o Jabba -l 20 -k 31 -p 2 -e 12 -g DBGraph.txt -fastq reads.fastq  

