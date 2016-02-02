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

#include "Settings.hpp"
#include "Seed.hpp"
#include "InterNodeChain.hpp"
#include "AlignedRead.hpp"
#include "Read.hpp"
#include "ReadCorrectionHandler.hpp"

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

void GraphChain::readGraph(ReadLibrary const &graph_input) {
	std::cout << "Reading the graph... " << std::endl;
        ReadFile *readFile = graph_input.allocateReadFile();
        readFile->open(graph_input.getInputFilename());
	while (true) {
                ReadRecord record;
                readFile->getNextRecord(record);
		std::vector<int> lnbs;
		std::vector<int> rnbs;
                extractNbs(record.preRead, lnbs, rnbs);
		graph_.addNode(record.read, lnbs, rnbs);
                if (!readFile->good())
                        break;
	}
	std::cout << "Done." << std::endl;
}


GraphChain::GraphChain(int argc, char * argv[]) :
	settings_(argc, argv), graph_(settings_)
{
	//read graph
	graph_.set_k(settings_.get_dbg_k());
        readGraph(settings_.get_graph());
	graph_.init_seed_finder();
        ReadCorrectionHandler rch(graph_, settings_);
        rch.doErrorCorrection(settings_.get_libraries());
}

int main(int argc, char * argv[]) {
	GraphChain gc(argc, argv);
	//gc.alignReads();
	return 0;
}
