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
#ifndef ALIGNEDREAD_HPP
#define ALIGNEDREAD_HPP


#include <string>

#include "LocalAlignment.hpp"
#include "Settings.hpp"

class Read;
class Graph;


struct AlignedRead {

	Read &read_; //reference to the read
	std::vector<LocalAlignment> local_alignments_; //list of alignments
	OutputMode output_mode_;

public:
	/*
	 *	ctors
	 */
	AlignedRead(Read &read, OutputMode output_mode);
	/*
	 *	methods
	 */
	//getters
	std::vector<LocalAlignment> get_local_alignments() {return local_alignments_;}
	//setters
	void set_local_alignments(std::vector<LocalAlignment> local_alignments) {local_alignments_ = local_alignments;}
	//get a list of all regions that have not been corrected
	std::vector<std::pair<int, int>> not_corrected();
	//add a list of local alignments
	void correct(std::vector<LocalAlignment> const &als);
	//add a local alignment
	void correct(LocalAlignment const &al);
	//correct the read based on the longest local alignment
	void getCorrectedRead(Graph const &graph,
		std::vector<std::string> &corrections);
	//sort local_alignments_
	void sortAlongRead();
	void sortReadCov();
	//checks if laj extends lai, overlap ends at index
	bool fit(std::vector<int>::iterator const &index,
		std::vector<int> const &lai, std::vector<int> const & laj);
	//merge local alignments
	bool merge();
	//print all local alignments
	void print();
};

#endif
