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
#ifndef SEED_HPP
#define SEED_HPP

class Seed{
	private:
		int node_; //node containing the seed
		int ref_start_; //offset in reference		
		int read_start_; //offset in read
		int length_; //length of the seed
	public:
		/*
		 *	ctors
		 */
		Seed(int node, int ref_start, int read_start, int length) : 
			node_(node),
			ref_start_(ref_start),
			read_start_(read_start),
			length_(length)
			{}
		/*
		 *	methods
		 */
		//getters
		int get_node() const {return node_;}
		int get_ref_start() const {return ref_start_;}
		int get_ref_end() const {return ref_start_ + length_;}
		int get_read_start() const {return read_start_;}
		int get_read_end() const {return read_start_ + length_;}
		int get_length() const {return length_;}
		//string representation, for debugging purposes
		std::string to_string() {
			std::string result = "";
			result += std::to_string(get_node());
			result += " ";
			result += std::to_string(get_ref_start());
			result += " ";
			result += std::to_string(get_read_start());
			result += " ";
			result += std::to_string(get_length());
			result += " ";
			return result;
		}
		//check if the seed is contained in the segment
		bool isInReadSegment(int segment_start, int segment_end){
			bool is_in_segment = segment_start <= get_read_start();
			is_in_segment *= get_read_end() <= segment_end;
			return is_in_segment;
		}
};

#endif
