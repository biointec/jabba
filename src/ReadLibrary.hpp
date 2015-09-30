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
#ifndef READLIBRARY_HPP
#define READLIBRARY_HPP

#include <vector>
#include <string>
#include <fstream>

class Read;
class Input;

class ReadLibrary{
	private:
		std::ifstream library_;
		std::string next_meta_;
		void (ReadLibrary:: *getNextRead) (std::string &, std::string &);
		void getNextFromFasta(std::string &meta, std::string &read);
		void getNextFromFastq(std::string &meta, std::string &read);
	public:
		ReadLibrary(){}
		ReadLibrary(Input const &library);
		void open(Input const &library);
		bool is_open() {return library_.is_open();}
		bool has_next() {return library_;}
		void close() {library_.close();}
		std::vector<Read> getReadBatch(int const &batch_size);
};
#endif
