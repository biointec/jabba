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
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "TNode.hpp"

class Graph{
	private:
		int size_; //number of nodes
		int k_; //size of k-mers, overlap between nodes is k-1
		std::vector<TNode> nodes_; //list of nodes
	public:
		/*
		 *	ctors
		 */
		Graph() : size_(0), k_(0) {
			TNode empty_node("", -1);
			nodes_.push_back(empty_node);
		}
		/*
		 *	methods
		 */
		//getters
		TNode get_node(int id) const {if (id > 0) {return nodes_[id];} else {return nodes_[-id];}}
		int get_size() const {return nodes_.size();}
		int get_overlap() const {return k_ - 1;}
		int get_k() const {return k_;}
		//setters
		void set_k(int k) {k_ = k;}
		//create a node with given sequence and neighbours
		void addNode(std::string const &sequence,
			std::vector<int> const &left_nb,
			std::vector<int> const &right_nb);
		//get the outedges of a node
		std::vector<int> getOutEdges(int node_id) const;
		//get the inedges of a node
		std::vector<int> getInEdges(int node_id) const;
		//get size of a node
		int getSizeOfNode(int node_id) const;
		//get sequence content of a node
		std::string getSequenceOfNode(int node_id) const;
		//get the sequence content of a path in the graph
		std::string concatenateNodes(std::vector<int> const &path) const;
		//print the size of a path in the graph
		void printPathLengths(int node);
		//extend along linear path
		void extendPathNosink(std::vector<int> &path, int est_dist, bool rev) const;
		//extend along linear path until sink is reached
		void extendPath(std::vector<int> &path, int sink, int est_dist, bool rev) const;
		//find a path between source and sink
		std::vector<int> findPath(int source, int sink, int org_est_dist) const;
		//limited dijkstra
		std::vector<int> findMinSeqLenPath(int source, int sink, int limit) const;
};
#endif
