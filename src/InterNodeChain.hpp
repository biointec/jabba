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
#ifndef INTERNODECHAIN_HPP
#define INTERNODECHAIN_HPP

#include <vector>
#include <map>

#include "Alignment.hpp"

class AlignedRead;
class Seed;
class Graph;
class SeedFinder;
class Read;
class InexactSeed;
class LocalAlignment;
class Settings;

class InterNodeChain{
	private:
		Read const &read_; //reference to the read
		Graph const &graph_; //reference to the graph
		Settings const &settings_; //reference to the settings
		std::map<int, std::vector<Seed>> seed_map_; //map containing all seeds
		std::vector<int> map_keys_; //list of all nodes that have seeds
		std::vector<int> seeds_of_size_; //number of seeds with a given length
		int max_passes_; //max number of passes of the algorithm
		Alignment &alignment_;
		/*
		 *	methods
		 */
		//check if seed is contained in other
		bool seedIsContainedInOther(InexactSeed const &seed,
			InexactSeed const &other) const;
		//check if seed is contained in any other seeds
		bool seedIsSuperMaximal(
			std::vector<InexactSeed> const &seeds,
			int const &index) const;
		//check if the inexact seed sufficiently covers the node
		bool seedCoversNode(InexactSeed seed) const;
		//check if the inexact seed is large enough
		bool seedIsSufficientlyBig(InexactSeed seed, int pass) const;
		//filter the seeds in global context
		void filterSeedsGlobal(
			std::vector<InexactSeed> &inexact_seeds,
			int pass) const;
		//count all the seeds contained in a segment of the read
		void countSeedsInSegment(
			int const &read_segment_start,
			int const &read_segment_end, 
			std::vector<int> &seeds_in_segment_of_size,
			int &seeds_in_segment);
		//find the size treshold for which seeds should be considered
		int findSeedTreshold(
			std::vector<int> &seeds_in_segment_of_size,
			int &seeds_in_segment, int const &seed_count_treshold)
			const;
		//get the list of nodes that should be chained
		std::vector<int> extractListOfNodesToChain(
			int const &read_segment_start,
			int const &read_segment_end,
			int const &seed_size_treshold);
		//organise the chaining of nodes within seeds
		std::vector<InexactSeed> organiseIntraNodeChaining(
			int const &segment_start, int const &segment_end,
			std::vector<int> probable_nodes);
		//filter the seeds locally
		std::vector<InexactSeed> filterSeedsLocal(
			std::pair<int, int> const &segment);
		//filter the seeds locally
		std::vector<InexactSeed> filterSeedsLocal(
			int const &read_segment_start,
			int const &read_segment_end);
		//align a possible correction to the read
		//returns the segment of the read that aligns
		std::pair<int, int> alignCorrectedToRead(std::string &corrected,
			std::string &read);
		//correct the read segment
		std::vector<LocalAlignment> correctRead(
			std::vector<InexactSeed> inexact_seeds);
		//chain the paths
		void chainPaths(AlignedRead &ar) const;
		//perform final chaining step by looking for paths in the graph
		void finalChaining(AlignedRead &ar) const;
		//filter small repeats
		void filterOverlappingSeeds(
			std::vector<InexactSeed> &inexact_seeds) const;
	public:
		/*
		 *	ctors
		 */
		InterNodeChain(Read const &read, Graph const &graph,
			Settings const &settings, Alignment &alignment);
		/*
		 *	methods
		 */
		//print all the seeds to std::cout
		void printSeeds();
		//chain seeds
		void chainSeeds(AlignedRead &ar,
			std::vector<std::string> &corrections);
};

#endif
