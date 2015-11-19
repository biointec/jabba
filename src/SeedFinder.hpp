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
#ifndef SEEDFINDER_HPP
#define SEEDFINDER_HPP

#include <vector>
#include <string>
#include <map>

class sparseSA;
class Seed;
class SeedFinder;
class Graph;

class SeedFinder{
	private:
		int min_length_; //min length of seeds
		sparseSA * sa_; //suffix array
		std::string reference_; //sparseSA requires the sequence from
					//which it is built to be kept in memory
		std::vector<int> nodes_index_; //list containing size of nodes
	public:
		/*
		 *	ctors
		 */
		SeedFinder(){};
		SeedFinder(std::string const &dir, Graph const &graph, int min_length, int k);
		void init(std::string const &dir, Graph const &graph, int min_length, int k);
		/*
		 *	dtors
		 */
		~SeedFinder();
		/*
		 *	methods
		 */
		//concatenate nodes and their reverse complements
		std::string preprocessGraph(Graph const &graph);
		//initialise the ESSA
		sparseSA * init_essaMEM(std::string &ref,
			std::string const &meta, int k);
		//find seeds between read and the graph
		void getSeeds(std::string read,
			std::map<int, std::vector<Seed>> &seed_map,
			std::vector<int> &seeds_of_size,
			std::vector<int> &map_keys, int &seed_count,
			int const &seed_min_length);
		//find the node in which a seed is contained
		int binary_node_search(int const &mem_start);
		//find where in the node the seed starts
		int startOfHit(int node_nr, int start_in_ref);
};
#endif
