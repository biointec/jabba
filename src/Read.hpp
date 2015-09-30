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
#ifndef READ_HPP
#define READ_HPP

#include <string>

#include "Sequence.hpp"

class Read : public Sequence {
private:
	int id_; //number of the read
	std::string meta_; //meta data
public:
	/*
	 *	ctors
	 */
	Read(std::string &meta, std::string const &sequence)
	 :  Sequence(sequence),
		id_(-1),
		meta_(meta)
		{}
	Read(int const &id, std::string &meta, std::string const &sequence)
	 :  Sequence(sequence),
		id_(id),
		meta_(meta)
		{}
	/*
	 *	methods
	 */
	//getters
	int get_id() const {return id_;}
	std::string get_meta() const {return meta_;}
	//setters
	void set_id(int id) {id_ = id;}
	void set_meta(std::string meta) {meta_ = meta;}
};

#endif
