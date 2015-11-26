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
#ifndef LOCALALIGNMENT_HPP
#define LOCALALIGNMENT_HPP

#include <vector>
#include <string>

class LocalAlignment {
	int read_start_; //start of read
	int read_end_; //end of read
	int ref_start_; //start of ref
	int ref_end_; //end of ref
	std::vector<int> path_; //list of nodes
public:
	/*
	 *	methods
	 */
	//getters
	int get_read_start() {return read_start_;}
	int get_read_end() {return read_end_;}
	int get_ref_start() {return ref_start_;}
	int get_ref_end() {return ref_end_;}
	std::vector<int> get_path() {return path_;}
	//setters
	void set_read_start(int read_start) {read_start_ = read_start;}
	void set_read_end(int read_end) {read_end_ = read_end;}
	void set_ref_start(int ref_start) {ref_start_ = ref_start;}
	void set_ref_end(int ref_end) {ref_end_ = ref_end;}
	void set_path(std::vector<int> path) {path_ = path;}
	
	std::string to_string(int read_id) {
		std::string str = "Read " + std::to_string(read_id)
			+ ": [" + std::to_string(read_start_)
			+ ", " + std::to_string(read_end_) + "] "
			+ " Ref: [" + std::to_string(ref_start_)
			+ ", " + std::to_string(ref_end_) + "] ";
		for (auto n : path_) {
			str += std::to_string(n) + " ";
		}
		return str;
	}
};
#endif
