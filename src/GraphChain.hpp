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
#ifndef GRAPHCHAIN_HPP
#define GRAPHCHAIN_HPP

#include <fstream>
#include <vector>
#include <string>

#include "Settings.hpp"
#include "ReadLibrary.hpp"
#include "SeedFinder.hpp"
#include "Graph.hpp"

class GraphChain {
	private:
		Settings settings_; //settings
		std::ofstream read_output_file_; //outputfile
		SeedFinder seed_finder_; //handles indexing and seed finding
		Graph graph_; //contains the de Bruijn graph
		/*
		 *	methods
		 */
		//extract the neighbours from the graph file
		std::vector<int> extractNbs(std::string arcs);
		//read the graph file
		void readGraph(std::string const &graph_file_name);
		//process the given input file
		void alignReads(Input const &library);
		//process a batch of reads
		std::vector<std::string> processBatch(
			std::vector<Read> const &reads, int read_count);
	public:
		/*
		 *	ctors
		 */
		GraphChain(int argc, char ** argv);
		/*
		 *	methods
		 */
		//process all input files
		void alignReads();
};

#endif
