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
#include "Graph.hpp"
#include "Nucleotide.hpp"

#include <deque>
#include <map>
#include <omp.h>

void Graph::addNode(std::string const &sequence, std::vector<int> const &in_edges, 
	std::vector<int> const &out_edges)
{
	TNode new_node("", nodes_.size(), sequence.size(), in_edges, out_edges);
	nodes_.push_back(new_node);
	seed_finder_.addNodeToReference(sequence);
	std::string rc_sequence = sequence;
	Nucleotide::revCompl(rc_sequence);
	seed_finder_.addNodeToReference(rc_sequence);
}

std::vector<int> Graph::getOutEdges(int node_id) const{
	if(node_id > 0){
		return nodes_[node_id].get_out_edges();
	} else {
		return nodes_[-node_id].get_rc_out_edges();
	}
}

std::vector<int> Graph::getInEdges(int node_id) const{
	if(node_id > 0){
		return nodes_[node_id].get_in_edges();
	} else {
		return nodes_[-node_id].get_rc_in_edges();
	}
}

int Graph::getSizeOfNode(int node_id) const{
	if (node_id == 0) {
		return k_ - 1;
	}
	if(node_id > 0){
		return nodes_[node_id].size();
	} else {
		return nodes_[-node_id].size();
	}
}

std::string Graph::getSequenceOfNode(int node_id) const {
	return seed_finder_.getNode(node_id);
}


std::string Graph::concatenateNodes(std::vector<int> const &path) const{
	if(path.size() == 0){
		return "";
	}
	std::string result = getSequenceOfNode(path[0]);
	for(int i = 1; i < path.size(); ++i){
		if (path[i] == 0) {
			continue;
		}
		std::vector<int> next = getOutEdges(path[i - 1]);
		if (std::find(next.begin(), next.end(), path[i]) == next.end()) {
			std::cout << "Path does not exist\n";
		}
		result += getSequenceOfNode(path[i]).substr(k_ - 1);
	}
	return result;
}

void Graph::extendPathNosink(std::vector<int> &path, int est_dist, bool rev = 0) const{
	return extendPath(path, 0, est_dist, rev);
}

void Graph::extendPath(std::vector<int> &path, int sink, int est_dist, bool rev = 0) const{
	bool tarfound = 0;
	while(true){
		std::vector<int> next;
		if(rev){
			next = getInEdges(path.back());
		} else {
			next = getOutEdges(path.back());
		}
		int good_next = -2;
		for(int i = 0; i < next.size(); ++i){
			if(std::find(path.begin(), path.end(), next[i]) != path.end()){
				return;
			}
			if(next[i] == sink || getSizeOfNode(next[i]) - (k_ - 1) < 2 * est_dist){
				if(good_next > -2){
					good_next = -1;
				} else {
					good_next = i;
				}
			}
		}
		if(good_next > -1){
			path.push_back(next[good_next]);
			if(!tarfound && next[good_next] == sink) {
				tarfound = 1;
			}
		} else {
			return;
		}
	}
}

std::vector<int> Graph::findPath(int source, int sink, int org_est_dist) const{
	int est_dist = org_est_dist;
	std::vector<int> path_i; //path from source
	std::vector<int> rev_path_t; //reverse path from sink
	//we always want to find a path from path_i.back to rev_path_t.back
	//a 0-value at position i means that the path is unknown between i-1 and i+1
	//the trivial correct solution is of course [source, 0, sink]
	path_i.push_back(source);
	rev_path_t.push_back(sink);
	//we try to extend the paths as much as possible
	int pi_len = 0;
	int rpt_len = 0;
	while(pi_len < path_i.size() || rpt_len < rev_path_t.size()){
		pi_len = path_i.size();
		rpt_len = rev_path_t.size();
		extendPath(path_i, sink, est_dist);
		extendPath(rev_path_t, source, est_dist, 1);
	}
	
	if(path_i.back() != sink){
		if(rev_path_t.back() != source){
			//we have not found a unique path yet
			int signed i = path_i.size() - 1;
			for(; -1 < i; --i){
				if(std::find(rev_path_t.begin(), rev_path_t.end(), path_i[i]) != rev_path_t.end()){
					//if the paths have a common node we will merge them
					break;
				}
			}
			if(i + 1 == path_i.size() && path_i.back() == rev_path_t.back()){
				std::vector<int> smallest_cycle = findMinSeqLenPath(path_i.back(), rev_path_t.back(), 2 * est_dist);
				int cycle_size = 0;
				for(int j = 1; j < smallest_cycle.size() - 1; ++j){
					cycle_size += getSizeOfNode(smallest_cycle[j]) - (k_ - 1);
				}
				if(est_dist * .8 - 10 < cycle_size && cycle_size < est_dist * 1.1 + 5){
					//cycle seems to fit
					path_i.insert(path_i.end(), smallest_cycle.begin() + 1, smallest_cycle.end() - 1);
					est_dist -= cycle_size;
				} else {
					if (est_dist > 50) {
						path_i.push_back(0);
					} else {
						path_i.pop_back(); //we collapse the two equal nodes, since the gap is so small
					}
					
				}
			} else if(i == -1){
				//if both paths never meet we concatenate them through node 0 (= unknown path)
				std::vector<int> smallest_path = findMinSeqLenPath(path_i.back(), rev_path_t.back(), 2 * est_dist);
				int path_size = 0;
				for(int j = 1; j < smallest_path.size() - 1; ++j){
					path_size += getSizeOfNode(smallest_path[j]) - (k_ - 1);
				}
				if(est_dist * .8 - 10 < path_size && path_size < est_dist * 1.1 + 5){
					//path seems to fit
					path_i.insert(path_i.end(), smallest_path.begin() + 1, smallest_path.end() - 1);
					est_dist -= path_size;
				} else {
					path_i.push_back(0);
				}
			} else {
				while(i + 1 < path_i.size()){
					path_i.pop_back();
				}
				while(rev_path_t.back() != path_i.back()){
					rev_path_t.pop_back();
				}
				rev_path_t.pop_back(); // else we add the match we found a second time!
			}
			while(!rev_path_t.empty()){
				path_i.push_back(rev_path_t.back());
				rev_path_t.pop_back();
			}
		} else {
			//we have found a unique path
			std::reverse(rev_path_t.begin(), rev_path_t.end());
			path_i = rev_path_t;
		}
	} else {
		//we have found a unique path, it is stored in path_i already
		
	}
	return path_i;
}

//dijkstra, but take special care if source == sink,
//we do not want trivial paths!
//considers seq length, not nodecount!
//the limit parameter is a hard cut-off for the length of allowed paths
std::vector<int> Graph::findMinSeqLenPath(int source, int sink, int limit) const{
	std::map<int/*node*/, std::pair<int/*dist*/, int/*prev*/>> nodes;
	std::deque<int> queue;
	//only relevant when source == sink
	int cycle_len = -1;
	int visited = 1;
	nodes[source] = (std::pair<int, int>(0, -1));
	queue.push_back(source);
	auto compare_nodes_dijkstra = [&](int a,int b)-> bool {
		return nodes[a].first < nodes[b].first;
	};
	int max_visited = 100;
	while(!queue.empty() && visited < max_visited){
		int node = queue.front();
		queue.pop_front();
		if(node == sink && (cycle_len > 0 || node != source)){
			//we are done
			break;
		}
		std::vector<int> nbs = getOutEdges(node);
		for(int i = 0; i < nbs.size(); ++i){
			int dist = nodes[node].first + getSizeOfNode(nbs[i]) - (k_ - 1);
			if(dist < limit){
				std::map<int, std::pair<int, int>>::iterator it = nodes.find(nbs[i]);
				if(it == nodes.end()){
					//nbs[i] has not yet been reached
					nodes[nbs[i]] = std::pair<int, int>(dist, node);
					queue.push_back(nbs[i]);
					++visited;
				} else {
					//nbs[i] has already been reached
					//no need to push to queue!
					if(dist < (*it).second.first){
						(*it).second.first = dist;
						(*it).second.second = node;
						
					}
					//we found a cycle
					if(source == sink && source == nbs[i]){
						if(cycle_len < 0 || dist < cycle_len){
							cycle_len = dist;
							(*it).second.second = node;
						}
					}
				}
			}
		}
			std::sort(queue.begin(), queue.end(), compare_nodes_dijkstra);
	}
	std::vector<int> path;
	path.push_back(sink);
	std::map<int, std::pair<int, int>>::iterator it = nodes.find(sink);
	if(it == nodes.end() || (source == sink && cycle_len == -1)){
		path.push_back(0);
		path.push_back(source);
	} else {
		//sink is found
		path.push_back((*it).second.second);
		while(path.back() != source){
			path.push_back(nodes[path.back()].second);
		}
	}
	std::reverse(path.begin(), path.end());
	return path;
}
