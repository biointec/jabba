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
#include "SeedFinder.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

#include "Seed.hpp"
#include "mummer/sparseSA.hpp"
#include "Input.hpp"

void SeedFinder::init () {
	preprocessReference();
	sa_ = init_essaMEM("DBGraph");
}

SeedFinder::~SeedFinder(){
	delete sa_;
}

void SeedFinder::addNodeToReference(std::string const &node) {
	reference_ += node;
	reference_ += "#";
	int size = nodes_index_.back() + node.size() + 1;
	nodes_index_.push_back(size);
}

void SeedFinder::preprocessReference() {
	// Increase string length so it is divisible by k_. 
	// Don't forget to count $ termination character. 
	int append_k;
	if(reference_.length() % k_ != 0) {
		append_k = k_ - reference_.length() % k_ ;
		
	}
	// Make sure last K-sampled characters are this special character as well!!
	append_k += k_;
	for(long i = 0; i < append_k; i++) {
		reference_ += "$";	// Append "special" end character. Note: It must be lexicographically less.
	}
}

int SeedFinder::binary_node_search(int const &mem_start) const {
	int signed left = 0;
	int signed mid;
	int signed right = nodes_index_.size();
	while(left + 1 != right){
		mid = (left + right) / 2;
		if(nodes_index_.at(mid) < mem_start){
			left = mid;
		} else if(nodes_index_.at(mid) > mem_start){
			right = mid;
		} else {
			left = mid;
			right = mid + 1;
		}
	}
	return (((left + 2) / 2)) * (left % 2 == 0 ? 1 : -1);
}

int SeedFinder::startOfHit(int node_nr, int start_in_ref) const {
	
	int start_of_node = nodes_index_[2 * node_nr * (node_nr < 0 ? -1 : 1) - 2 + (node_nr < 0)];
	return start_in_ref - start_of_node;
}
void SeedFinder::getSeeds(std::string const &read,
	std::map<int, std::vector<Seed>> &seed_map,
	std::vector<int> &seeds_of_size, std::vector<int> &map_keys,
	int &seed_count, int const &seed_min_length) const
{
	//mem finding
	vector<match_t> matches;	//will contain the matches
	bool print = 0;	//not sure what it prints if set to 1
	sa_->findMEM(0, read, matches, seed_min_length, print);
		
	//parse the results
	for (int i = 0; i < matches.size(); ++i) {
		match_t m = matches[i];
		int node_nr = binary_node_search(m.ref);
		int node_start = startOfHit(node_nr, m.ref);
		seed_map[node_nr].push_back(Seed(node_nr, node_start, m.query, m.len));
		while (seeds_of_size.size() <= m.len) {
			seeds_of_size.push_back(0);
		}
		++seeds_of_size[m.len];
		++seed_count;
		if (std::find(map_keys.begin(), map_keys.end(), node_nr) == map_keys.end()) {
			map_keys.push_back(node_nr);
		}
	}
}

sparseSA * SeedFinder::init_essaMEM(std::string const &meta) {	
	std::cout << "Constructing ESSA... " << std::endl;
	std::vector<std::string> refdescr;
	refdescr.push_back(meta);
	std::vector<long> startpos;
	startpos.push_back(0); //only one reference
	bool printSubstring = false;
	bool printRevCompForw = false;
	int sparseMult = 1;
	bool suflink = true;
	bool child = false;
	sparseSA * sa;
	sa = new sparseSA(reference_,
		refdescr,
		startpos,
		false,
		k_,
		suflink,
		child,
		false,
		sparseMult,
		0,
		printSubstring,
		printRevCompForw,
		false
	);
	//sa->construct();
	
	stringstream * prefixstream = new stringstream();
	(*prefixstream) << settings_.get_directory() << "/" << meta << "_" << k_ << "_" << suflink << "_" << child;
	string prefix = prefixstream->str();
	if(!sa->load(prefix)) {
		sa->construct();
		sa->save(prefix);
	}
	delete prefixstream;
	std::cout << "Done." << std::endl;
	std::cout << "INDEX SIZE IN BYTES: " << sa->index_size_in_bytes() << endl;
	return sa;
}

