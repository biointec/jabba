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
#include "InterNodeChain.hpp"

#include "Graph.hpp"
#include "SeedFinder.hpp"
#include "IntraNodeChain.hpp"
#include "Read.hpp"
#include "Settings.hpp"
#include "AlignedRead.hpp"

//ctors

InterNodeChain::InterNodeChain(Read const &read, Graph const &graph,
	Settings const &settings, Alignment &alignment)
 :	read_(read),
	graph_(graph),
	settings_(settings),
	seed_map_(),
	map_keys_(),
	seeds_of_size_(),
	max_passes_(settings.get_max_passes()),
	alignment_(alignment)
{
	int seed_min_len = settings_.get_min_len();
	int seed_count = 0;
	seeds_of_size_.clear();
	map_keys_.clear();
	seed_map_.clear();
	graph_.getSeeds(read_.get_sequence(), seed_map_, seeds_of_size_,
		map_keys_, seed_count, seed_min_len);
	//printSeeds();
}

//methods

void InterNodeChain::printSeeds() {
	for (int j = 0; j < map_keys_.size(); ++j) {
		for (int k = 0; k < seed_map_[map_keys_[j]].size(); ++k) {
			Seed s = seed_map_[map_keys_[j]][k];
			std::cerr << s.to_string() << std::endl;
		}
	}
}

bool InterNodeChain::seedIsContainedInOther(InexactSeed const &seed,
	InexactSeed const &other) const
{
	return (other.get_read_start() <= seed.get_read_start()
			&& seed.get_read_end() <= other.get_read_end());
}

bool InterNodeChain::seedIsSuperMaximal(
	std::vector<InexactSeed> const &seeds, int const &index) const
{
	if (index > 0) {
		if (seedIsContainedInOther(seeds[index], seeds[index - 1])) {
			return 0;
		}
	}
	if (index + 1 < seeds.size()) {
		if (seedIsContainedInOther(seeds[index], seeds[index + 1])) {
			return 0;
		}
	}
	return 1;
}

//this function determines if an inexact seed gives a realistic covering
//of a node, given the error profile and minimal MEM size
bool InterNodeChain::seedCoversNode(InexactSeed seed) const {
	//start of the (crudely) estimated covered region c
	int c_start = seed.get_node_start() - seed.get_read_start();
	c_start = c_start > 0 ? c_start : 0;
	//end of c
	int c_end = seed.get_node_end()
		+ (read_.get_sequence().size() - seed.get_read_end());
	c_end = c_end < graph_.getSizeOfNode(seed.get_node())
			? c_end : graph_.getSizeOfNode(seed.get_node());
	return 5 * (seed.get_node_end() - seed.get_node_start()) > (c_end - c_start);
}

bool InterNodeChain::seedIsSufficientlyBig(InexactSeed seed, int pass) const {
	return seed.get_node_end() - seed.get_node_start() > 50 - 5 * pass;
}

void InterNodeChain::filterSeedsGlobal(std::vector<InexactSeed> &inexact_seeds,
	int pass) const
{
	std::sort(inexact_seeds.begin(), inexact_seeds.end());
	std::vector<int> remove_seed;
	//first we loop through the seeds to see which ones are worth keeping
	for (int index = 0; index < inexact_seeds.size(); ++index) {
		bool seed_is_high_qual = seedIsSuperMaximal(inexact_seeds, index);
		seed_is_high_qual *= seedCoversNode(inexact_seeds[index]);
		seed_is_high_qual *= seedIsSufficientlyBig(inexact_seeds[index], pass);
		if (!seed_is_high_qual) {
			remove_seed.push_back(index);
		}
	}
	//second loop removes all those that we do not want to keep
	//iterate from back to front so we can safely use indices without adaptions
	for (int i = remove_seed.size() - 1; i > -1; --i) {
		int index = remove_seed[i];
		inexact_seeds.erase(inexact_seeds.begin() + index);
	}
}

void InterNodeChain::countSeedsInSegment(int const &read_segment_start,
	int const &read_segment_end, std::vector<int> &seeds_in_segment_of_size,
	int &seeds_in_segment)
{
	for (int j = 0; j < map_keys_.size(); ++j) {
		for (int k = 0; k < seed_map_[map_keys_[j]].size(); ++k) {
			Seed s = seed_map_[map_keys_[j]][k];
			if (s.isInReadSegment(read_segment_start, read_segment_end)) {
				while (seeds_in_segment_of_size.size() < s.get_length() + 1) {
					seeds_in_segment_of_size.push_back(0);
				}
				++seeds_in_segment_of_size[s.get_length()];
				++seeds_in_segment;
			}
		}
	}
}

int InterNodeChain::findSeedTreshold(
	std::vector<int> &seeds_in_segment_of_size,
	int &seeds_in_segment, int const &seed_count_treshold) const
{
	int seed_size_treshold = seeds_in_segment_of_size.size() - 1;
	int seeds_past_treshold = 0;
	for (; seed_size_treshold > 0; --seed_size_treshold) {
		seeds_past_treshold += seeds_in_segment_of_size[seed_size_treshold];
		if (seeds_past_treshold > seed_count_treshold
			|| seeds_past_treshold > seeds_in_segment / 2) {
			break;
		}
	}
	return seed_size_treshold;
}

std::vector<int> InterNodeChain::extractListOfNodesToChain(
	int const &read_segment_start, int const &read_segment_end, 
	int const &seed_size_treshold)
{
	std::vector<int> probable_nodes;
	for (int j = 0; j < map_keys_.size(); ++j) {
		for (int k = 0; k < seed_map_[map_keys_[j]].size(); ++k) {
			Seed s = seed_map_[map_keys_[j]][k];
			if (s.isInReadSegment(read_segment_start, read_segment_end)
				&& s.get_length() >= seed_size_treshold) {
				probable_nodes.push_back(map_keys_[j]);
				break;
			}
		}
	}
	return probable_nodes;
}


std::vector<InexactSeed> InterNodeChain::organiseIntraNodeChaining(
	int const &segment_start, int const &segment_end,
	std::vector<int> probable_nodes)
{
	std::vector<InexactSeed> inexact_seeds;
	for (int j = 0; j < probable_nodes.size(); ++j) {
		std::vector<Seed> seeds_in_segment;
		for (int k = 0; k < seed_map_[probable_nodes[j]].size(); ++k) {
			Seed s = seed_map_[probable_nodes[j]][k];
			if (s.isInReadSegment(segment_start, segment_end)) {
				seeds_in_segment.push_back(s);
			}
		}
		IntraNodeChain ianc(seeds_in_segment, settings_);
		ianc.groupSeedsInNode(inexact_seeds);
	}
	return inexact_seeds;
}

//filters seeds within nodes
std::vector<InexactSeed> InterNodeChain::filterSeedsLocal(
	std::pair<int, int> const &segment)
{
	return filterSeedsLocal(segment.first, segment.second);
}

std::vector<InexactSeed> InterNodeChain::filterSeedsLocal(
		int const &read_segment_start,
		int const &read_segment_end) {
		//(1) find the biggest MEMs
	std::vector<int> seeds_in_segment_of_size;
	int seeds_in_segment = 0;
	countSeedsInSegment(read_segment_start, read_segment_end,
		seeds_in_segment_of_size, seeds_in_segment);
	int seed_size_treshold = findSeedTreshold(seeds_in_segment_of_size,
		seeds_in_segment, 5 + 10 * (0 == read_segment_start));
	std::vector<int> probable_nodes = extractListOfNodesToChain(
		read_segment_start, read_segment_end, seed_size_treshold);
		//(2) locally chain the nodes containing these MEMs
	std::vector<InexactSeed> inexact_seeds = organiseIntraNodeChaining(
		read_segment_start, read_segment_end, probable_nodes);
	return inexact_seeds;
}

std::pair<int, int> InterNodeChain::alignCorrectedToRead(std::string &corrected,
	std::string &read)
{
	//std::reverse(read.begin(), read.end());
	//std::reverse(corrected.begin(), corrected.end());
	int size = 200;
	if (size > read.size()) {
		size = read.size();
	}
	if (size > corrected.size()) {
		size = corrected.size();
	}
	std::string read_p = read.substr(0, size);
	std::string corrected_p = corrected.substr(0, size);
	std::cout << read_p << std::endl;
	std::cout << corrected_p << std::endl;
	int score = alignment_.align(read_p, corrected_p);
	std::cout << "score: " << score << std::endl;
	alignment_.printAlignment(read_p, corrected_p);
	
	return std::pair<int, int> (0, 0);
}


//when several global seeds have nontrivial overlap, we remove them
void InterNodeChain::filterOverlappingSeeds(
	std::vector<InexactSeed> &inexact_seeds) const
{
	for (int i = 0; i < inexact_seeds.size(); ++i) {
		InexactSeed isi = inexact_seeds[i];
		std::vector<int> next = graph_.getOutEdges(isi.get_node());
	}
	std::vector<InexactSeed> filtered;
	for (int i = 0; i < inexact_seeds.size(); ++i) {
		InexactSeed curr_is = inexact_seeds[i];
		int j = 0;
		for (; j < inexact_seeds.size(); ++j) {
			if (i == j) {
				continue;
			}
			InexactSeed other_is = inexact_seeds[j];
			if (other_is.get_read_start() - 2 < curr_is.get_read_start()
				&& curr_is.get_read_end() < other_is.get_read_end() + 2)
			{
				break;
			}
		}
		if (j == inexact_seeds.size()) {
			filtered.push_back(curr_is);
		}
	}
	inexact_seeds = filtered;
	for (int i = 0; i < inexact_seeds.size(); ++i) {
		InexactSeed isi = inexact_seeds[i];
		std::vector<int> next = graph_.getOutEdges(isi.get_node());
	}
}

std::vector<LocalAlignment> InterNodeChain::correctRead(
	std::vector<InexactSeed> inexact_seeds)
{
	std::vector<LocalAlignment> alignments;
	for (int i = 0; i < inexact_seeds.size(); ++i) {
		InexactSeed is = inexact_seeds[i];
		LocalAlignment la;


		std::vector<int> pre_path;
		pre_path.push_back(is.get_node());
		graph_.extendPath(pre_path, 0, 1000000, true);
		std::reverse(pre_path.begin(), pre_path.end());
		std::string pre_path_seq = graph_.concatenateNodes(pre_path);
		//std::cout << "Inexact Seed: " << is.get_node() << " " << is.get_node_start() << " " << is.get_node_end() << " " << is.get_read_start() << " " << is.get_read_end() << std::endl;
		int pre_path_size = pre_path_seq.size() - (graph_.getSizeOfNode(is.get_node()) - is.get_node_start());
		pre_path_seq = pre_path_seq.substr(0, pre_path_size);
		if (is.get_read_start() > pre_path_seq.size()) {
			//std::string r = read_.get_sequence().substr(0, is.get_read_start());
			//la.set_read_start(alignCorrectedToRead(pre_path_seq, r).first);
			la.set_read_start(is.get_read_start()); //TODO
			la.set_ref_start(pre_path_seq.size());
		} else {
			la.set_read_start(0);
			la.set_ref_start(pre_path_seq.size() - is.get_read_start());
		}


		std::vector<int> post_path;
		post_path.push_back(is.get_node());
		graph_.extendPath(post_path, 0, 1000000, false);
		std::string post_path_seq = graph_.concatenateNodes(post_path);
		int post_path_start = is.get_node_end();
		if (post_path_start < post_path_seq.size()) {
			post_path_seq = post_path_seq.substr(post_path_start);
		} else {
			post_path_seq = "";
		}
		if (read_.size() - is.get_read_end() >= post_path_seq.size()) {
			//std::string r = read_.get_sequence().substr(is.get_read_end());
			//la.set_read_end(alignCorrectedToRead(pre_path_seq, r).second);
			la.set_read_end(is.get_read_end()); //TODO
			la.set_ref_end(la.get_ref_start() + is.get_read_end() - is.get_read_start());
		} else {
			la.set_read_end(read_.size());
			la.set_ref_end(la.get_ref_start() + read_.size());
		}
		


		for (int j = 1; j < post_path.size(); ++j) {
			pre_path.push_back(post_path[j]);
		}
		la.set_path(pre_path);
		//std::cout << "Local Alignment: " << la.to_string(read_.get_id()) << std::endl;
		alignments.push_back(la);
	}
	return alignments;
}

std::string InterNodeChain::chainSeeds(AlignedRead &ar) {
	for (int i = 0; i < max_passes_; ++i) {
		std::vector<std::pair<int, int>> segments
			= ar.not_corrected();
		for (int j = 0; j < segments.size(); ++j) {
			std::vector<InexactSeed> inexact_seeds
				= filterSeedsLocal(segments[j]);
			//std::cout << inexact_seeds.size() << " inexact seeds initial\n";
			if (inexact_seeds.size() == 0) {
				continue;
			}
			filterSeedsGlobal(inexact_seeds, i);
			//std::cout << inexact_seeds.size() << " inexact seeds remain\n";
			if (inexact_seeds.size() == 0) {
				continue;
			}
			//filterOverlappingSeeds(inexact_seeds);
			//std::cout << inexact_seeds.size() << " inexact seeds remain\n";
			if (inexact_seeds.size() == 0) {
				continue;
			}
			std::vector<LocalAlignment> alignments
				= correctRead(inexact_seeds);
			ar.correct(alignments);
		}
		//chainPaths(ar);
	}
	//chainPaths(ar);
	std::string result = ar.getCorrectedRead(graph_);
	return result;
}

void InterNodeChain::chainPaths(AlignedRead &ar) const {
	ar.sort();
	std::vector<LocalAlignment> las = ar.get_local_alignments();
	if (las.size() < 2) {
		return;
	}
	LocalAlignment curr_la = las[0];
	for (int i = 1; i < las.size(); ++i) {
		LocalAlignment prev_la = curr_la;
		curr_la = las[i];
		LocalAlignment la;
		//find path
		int source = prev_la.get_path().back();
		int sink = curr_la.get_path()[0];
		int dist = curr_la.get_read_start() - prev_la.get_read_end();
		std::vector<int> path = graph_.findMinSeqLenPath(
			source, sink, 2 * dist);
		if (std::find(path.begin(), path.end(), 0) != path.end()) {
			continue;
		}
		//std::cout << "path found\n";
		std::vector<int> new_path = prev_la.get_path();
		std::vector<int> next_path = curr_la.get_path();
		for (int j = 1; j < path.size(); ++j) {
			new_path.push_back(path[j]);
		}
		for (int j = 1; j < next_path.size(); ++j) {
			new_path.push_back(next_path[j]);
		}
		prev_la.set_path(new_path);
		prev_la.set_read_end(curr_la.get_read_end());
		prev_la.set_ref_end(prev_la.get_ref_start() + prev_la.get_read_end() - prev_la.get_read_start());
		las[i - 1] = prev_la;
		las.erase(las.begin() + i);
		--i;
	}
	ar.set_local_alignments(las);
}

