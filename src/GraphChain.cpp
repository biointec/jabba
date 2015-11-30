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
#include "GraphChain.hpp"

#include <sstream>
#include <omp.h> //openmp

#include "Settings.hpp"
#include "Seed.hpp"
#include "InterNodeChain.hpp"
#include "AlignedRead.hpp"
#include "Read.hpp"


void GraphChain::extractNbs(std::string const &arcs, std::vector<int> &lnbs, std::vector<int> &rnbs) {
	std::istringstream iss(arcs);
	std::string arcs_count;
	iss >> arcs_count; //>NODE
	iss >> arcs_count; //<nodenr>
	iss >> arcs_count; //<size>
	iss >> arcs_count; //store number of arcs
	for (int i = 0; i < std::stoi(arcs_count); ++i) {
		std::string nb;
		iss >> nb;
		lnbs.push_back(std::stoi(nb));
	}
	iss >> arcs_count; //store number of arcs
	for (int i = 0; i < std::stoi(arcs_count); ++i) {
		std::string nb;
		iss >> nb;
		rnbs.push_back(std::stoi(nb));
	}
}

void GraphChain::readGraph(Input const &graph_input) {
	std::cout << "Reading the graph... " << std::endl;
	ReadLibrary graph(graph_input);
	if (!graph.is_open()) {
		std::cerr << std::endl << "Error: file " << graph_input.filename_
			<< " is not open." << std::endl;
		exit(1);
	}
	while (graph.has_next()) {
		std::vector<Read> nodes;
		graph.getReadBatch(nodes, 1);
		Read read = nodes[0];
		std::vector<int> lnbs;
		std::vector<int> rnbs;
		extractNbs(read.get_meta(), lnbs, rnbs);
		graph_.addNode(read.get_sequence(), lnbs, rnbs);
	}
	std::cout << "Done." << std::endl;
}

void GraphChain::alignReads() {
	for (Input library : settings_.get_input()) {
		std::cout << library.basename_ << std::endl;
		alignReads(library);
	}
}

void GraphChain::alignReads(Input const &library) {
	//open read input files
	ReadLibrary read_library(library);
	std::string of_name = settings_.get_directory() + "/Jabba-"
		+ library.basename_;
	std::string ucf_name = settings_.get_directory() + "/Jabba_uncorrected-"
		+ library.basename_;
	int lastindex = of_name.find_last_of(".");
	if (lastindex != std::string::npos) {
		of_name = of_name.substr(0, lastindex) + ".fasta";
	} else {
		of_name = of_name + ".fasta";
	}
	std::ofstream read_output_file;
	read_output_file.open(of_name.c_str());
	std::ofstream read_uncorrected_file;
	read_uncorrected_file.open(ucf_name.c_str());
	if (!read_library.is_open()) {
		std::cerr << std::endl << "Error: file " << library.filename_
			<< " is not open." << std::endl;
		exit(1);
	}
	if (!read_output_file.is_open()) {
		std::cerr << std::endl << "Error: file " << of_name << " is not open." << std::endl;
		exit(1);
	}
	if (!read_uncorrected_file.is_open()) {
		std::cerr << std::endl << "Error: file " << ucf_name << " is not open." << std::endl;
		exit(1);
	}
	//reads some reads from file and then processes them
	int batch_nr = 0;
	int read_count = 0;
	while (read_library.has_next()) {
		batch_nr++;
		int batch_size = 1024;
		std::cout << "Reading batch " << batch_nr << std::endl;
		std::vector<Read> reads;
		read_library.getReadBatch(reads, batch_size);
		std::vector<std::string> corrected_reads = processBatch(reads,
			read_count);
		batch_size = reads.size();
		for (int i = 0; i < batch_size; ++i) {
			if (corrected_reads[i].size() > 0) {
				read_output_file << ">" << reads[i].get_meta() << std::endl
				<< corrected_reads[i] << std::endl;
			} else {
				read_uncorrected_file << ">" << reads[i].get_meta() << std::endl
				<< reads[i].get_sequence() << std::endl;
			}
		}
		read_count += batch_size;
	}
	read_uncorrected_file.close();
	read_output_file.close();
}

std::vector<std::string> GraphChain::processBatch(
	std::vector<Read> const &reads, int read_count)
{
	std::cout << "Processing batch of size: " << reads.size() << std::endl;
	int batch_size = reads.size();
	std::vector<std::string> corrected_reads;
	corrected_reads.resize(batch_size);
	#pragma omp parallel
	{
		int reads_per_thread = (batch_size / omp_get_num_threads());
			if(batch_size % omp_get_num_threads() >= omp_get_thread_num()){
			++reads_per_thread;
		}
		int local_read_count = omp_get_thread_num() * reads_per_thread;
		if(batch_size % omp_get_num_threads() == omp_get_thread_num()){
			--reads_per_thread;
		}
		if(batch_size % omp_get_num_threads() < omp_get_thread_num()){
			reads_per_thread += batch_size % omp_get_num_threads();
		}
		for (int i = 0; i < reads_per_thread; ++i) {
			Read read = reads[local_read_count + i];
			read.set_id(read_count + local_read_count + i);
			std::cout << "Processing read " << read.get_id() << std::endl;
			std::cerr << "Processing read " << read.get_id() << std::endl;
			Alignment alignment(250, 30, 1, -4, -2, -3);
			InterNodeChain iernc(read, graph_, settings_, alignment);
			AlignedRead ar(read, settings_.get_output_mode());
			corrected_reads[read.get_id() - read_count] = iernc.chainSeeds(ar);
		}
	}
	return corrected_reads;
}

GraphChain::GraphChain(int argc, char * argv[]) :
	settings_(argc, argv), graph_(settings_)
{
	//read graph
	Input graph = settings_.get_graph();
	graph_.set_k(settings_.get_dbg_k());
	readGraph(graph);
	graph_.init_seed_finder();
	omp_set_num_threads(settings_.get_num_threads());
}

int main(int argc, char * argv[]) {
	GraphChain gc(argc, argv);
	gc.alignReads();
	return 0;
}
