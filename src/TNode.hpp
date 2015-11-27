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
#ifndef NODE_HPP
#define NODE_HPP

#include "TString.hpp"

class TNode {
private:
	int id_;			//node ID
	int size_;			//size
	//TString sequence_;		//DNA sequence of the node
	//TString rc_sequence_;		//DNA sequence of the rc node
	std::vector<int> in_edges_;	//edges that enter the node
	std::vector<int> out_edges_;	//edges that leave the node
public:
	/*
	 *	ctors
	 */
	TNode(std::string const &sequence, int const &id, int const &size)
	      :	id_(id),
		size_(size)
	{}
	TNode(std::string const &sequence, int const &id, int const &size,
		 std::vector<int> const &in_edges, 
		 std::vector<int> const &out_edges)
	      :	id_(id),
		size_(size),
		in_edges_(in_edges),
		out_edges_(out_edges)
	{}
	TNode(TNode const &tnode)
	      :	id_(tnode.get_id()),
		size_(tnode.size()),
		in_edges_(tnode.get_in_edges()),
		out_edges_(tnode.get_out_edges())
	{}
	/*
	 *	methods
	 */
	//static function that negates a list of nodes
	static std::vector<int> reverseEdges(std::vector<int> edge_list) {
		for (int i = 0; i < edge_list.size(); ++i) {
			edge_list[i] = -edge_list[i];
		}
		return edge_list;
	}
	//getters
	int get_id() const {return id_;}
	std::vector<int> get_in_edges() const {return in_edges_;}
	std::vector<int> get_rc_in_edges() const {return reverseEdges(out_edges_);}
	std::vector<int> get_out_edges() const {return out_edges_;}
	std::vector<int> get_rc_out_edges() const {return reverseEdges(in_edges_);}
	
	int size() const {return size_;};
};

#endif
