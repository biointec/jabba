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
#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <string>
#include <vector>

typedef std::vector<bool> twobit;

class Sequence {
private:
	twobit seq_; //twobit encoding of ACGT string
	int size_; //length of the sequence
public:
	/*
	 *	ctors
	 */
	Sequence() {}
	Sequence(std::string const &seq);
	/*
	 *	methods
	 */
	//string2twobit
	static twobit encode(std::string const &decoded);
	//twobit2string
	static std::string decode(twobit const &encoded);
	//twobit reversecomplement
	static twobit reverseComplement(twobit const &encoded);
	//string reversecomplement
	static std::string reverseComplement(std::string const &sequence);
	//change bases other than ACGT in ACGT, priority is A>C>G>T
	static std::string collapse_to_acgt(std::string const &seq);
	//getters
	twobit get_seq() const {return seq_;}
	twobit get_rc_seq() const {return reverseComplement(seq_);}
	std::string get_rc_sequence() const {return decode(get_rc_seq());}
	std::string get_sequence() const {return decode(seq_);}
	int size() const {return size_;}
	//setters
	void set_sequence(std::string const &seq);
};

#endif
