/*******************************************************************************
 *   Copyright (C) 2014, 2015 Giles Miclotte (giles.miclotte@intec.ugent.be)   *
 *   This file is part of Jabba                                                *
 *                                                                             *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by      *
 *   the Free Software Foundation; either version 2 of the License, or         *
 *   (at your option) any later version.                                       *
 *                                                                             *
 *   This program is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *                                                                             *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program; if not, write to the                             *
 *   Free Software Foundation, Inc.,                                           *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 *
 *******************************************************************************/
#include "Settings.hpp"

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <thread>

Settings::Settings(int argc, char** args)
       :graph_(FASTA, "DBGraph.fasta")
{
	// parse program arguments
	if (argc < 4) {
		if (argc == 1) {
			printProgramInfo();
		} else {
			std::string arg = args[1];
			if (arg == "-h" || arg == "--help") {
				printUsage();
				exit(EXIT_SUCCESS);
			} else if (arg == "-i" || arg == "--info") {
				printProgramInfo();
				exit(EXIT_SUCCESS);
			}
		}
		printUsage();
		exit(EXIT_FAILURE);
	}

	//set standard values
	num_threads_ = std::thread::hardware_concurrency();
	essa_k_ = 1;
	max_passes_ = 2;
	min_len_ = 20;
	directory_ = "Jabba_output";
	output_mode_ = LONG;

	FileType file_type(FASTA);
	// extract the other parameters
	for (int i = 1; i < argc; i++) {
		std::string arg(args[i]);
		if (arg.empty()) continue;// this shouldn't happen
		if (arg == "-fastq") { // file type
			std::cout << "FileType is fastq." << std::endl;
			file_type = FASTQ;
 		} else if (arg == "-fasta") {
			std::cout << "FileType is fasta." << std::endl;
			file_type = FASTA;
		} else if (arg == "-t" || arg == "--threads") {
			++i;
			num_threads_ = std::stoi(args[i]);
		} else if (arg == "-g" || arg == "--graph") {
			++i;
			graph_ = Input(FASTA, args[i]);
		} else if (arg == "-k" || arg == "--dbgk") {
			++i;
			dbg_k_ = std::stoi(args[i]);
		} else if (arg == "-e" || arg == "--essak") {
			++i;
			essa_k_ = std::stoi(args[i]);
		} else if (arg == "-p" || arg == "--passes") {
			++i;
			max_passes_ = std::stoi(args[i]);
		} else if (arg == "-l" || arg == "--length") {
			++i;
			min_len_ = std::stoi(args[i]);
		} else if (arg == "-o" || arg == "--output") {
			++i;
			directory_ = args[i];
		} else if (arg == "-s" || arg == "--short") {
			output_mode_ = SHORT;
		} else {// filename
			inputs_.push_back(Input(file_type, arg));
		}
	}
	// try to create the output directory
	#ifdef _MSC_VER
	CreateDirectory(directory_.c_str(), NULL);
	#else
	DIR * dir = opendir(directory_.c_str());
	if ((dir == NULL) && (mkdir(directory_.c_str(), 0777) != 0))
		throw std::ios_base::failure("Can't create directory: " + directory_);
	closedir(dir);
	#endif
	// log the instructions
	logInstructions(argc, args);
	//print settings
	std::cout << "Max Number of Threads is " << num_threads_ << std::endl;
	std::cout << "Graph is " << graph_.basename_ << std::endl;
	std::cout << "DBG K is " << dbg_k_ << std::endl;
	std::cout << "ESSA K is " << essa_k_ << std::endl;
	std::cout << "Max Passes is " << max_passes_ << std::endl;
	std::cout << "Min Seed Size is " << min_len_ << std::endl;
	std::cout << "Output Directory is " << directory_ << std::endl;
	std::cout << "Output Mode is ";
	if (output_mode_ == SHORT){
		std::cout << "short" << std::endl;
	} else if (output_mode_ == LONG){
		std::cout << "long" << std::endl;
	} else {
		std::cout << "not implemented" << std::endl;
	}
}

void Settings::printProgramInfo() const {
	std::cout << "Jabba: Hybrid Error Correction." << std::endl;
	std::cout << "Jabba v." << JABBA_MAJOR_VERSION << "."
	     << JABBA_MINOR_VERSION << "." << JABBA_PATCH_LEVEL << "\n";
	std::cout << "Copyright 2014, 2015 Giles Miclotte (giles.miclotte@intec.ugent.be)\n";
	std::cout << "This is free software; see the source for copying conditions. "
		"There is NO\nwarranty; not even for MERCHANTABILITY or "
		"FITNESS FOR A PARTICULAR PURPOSE.\n" << std::endl;
	std::cout << std::endl;
}

void Settings::printUsage() const {
	std::cout << "Usage: Jabba [options] [file_options] file1 [[file_options] file2]...\n";
	std::cout << "Corrects sequence reads in file(s)\n\n";
	std::cout << " [options]\n";
	std::cout << "  -h\t--help\t\tdisplay help page\n";
	std::cout << "  -i\t--info\t\tdisplay information page\n";
	std::cout << " [options arg]\n";
	std::cout << "  -l\t--length\tminimal seed size [default = 20]\n";
	std::cout << "  -k\t--dbgk\t\tde Bruijn graph k-mer size\n";
	std::cout << "  -e\t--essak\t\tsparseness factor of the enhance suffix array [default = 1]\n";
	std::cout << "  -t\t--threads\tnumber of threads [default = available cores]\n";
	std::cout << "  -p\t--passes\tmaximal number of passes per read [default = 2]\n";
	std::cout << "  -s\t--short\tdo not extend the reads\n";
	std::cout << " [file_options file_name]\n";
	std::cout << "  -o\t--output\toutput directory [default = Jabba_output]\n";
	std::cout << "  -fastq\t\tfastq input files\n";
	std::cout << "  -fasta\t\tfasta input files\n";
	std::cout << "  -g\t--graph\t\tgraph input file [default = DBGraph.fasta]\n\n";
	std::cout << " examples:\n";
	std::cout << "  ./Jabba --dbgk 31 --graph DBGraph.txt -fastq reads.fastq\n";
	std::cout << "  ./Jabba -o Jabba -l 20 -k 31 -p 2 -e 12 -g DBGraph.txt -fastq reads.fastq\n";
}

std::string Settings::getLogFilename() const {
	return directory_ + "/log";
}

void Settings::logInstructions(int argc, char **args) const {
	std::ofstream logFile;
	logFile.open(getLogFilename().c_str());
	if (!logFile)
		throw std::ios_base::failure("Can't write to " + getLogFilename());
	for (int i = 0; i < argc; i++)
		logFile << " " << args[i];
	logFile << std::endl;
	logFile.close();
}
