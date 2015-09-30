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
#include "ReadLibrary.hpp"

#include "Read.hpp"
#include "Input.hpp"

void ReadLibrary::getNextFromFasta(std::string &meta, std::string &read){
	meta = next_meta_;
	read = "";
	std::string partial_read;
	if(library_){
		getline(library_, partial_read);
	}
	while(library_ && (partial_read.size() == 0 || partial_read[0] != '>')){
		read += partial_read;
		getline(library_, partial_read);
	}
	next_meta_ = partial_read;
}

void ReadLibrary::getNextFromFastq(std::string &meta, std::string &read){
	meta = next_meta_;
	std::string partial_read;
	if(library_){
		//this is the actual read
		getline(library_, read);
	}
	for(int i = 0; i < 3; ++i){
		if(library_){
			getline(library_, next_meta_);
		} else {
			//end of file reached
			next_meta_ = "";
		}
	}
}

ReadLibrary::ReadLibrary(Input const &library)
{
	open(library);
}

void ReadLibrary::open(Input const &library) {
	library_.open(library.filename_);
	//set which function should be used to read the next line
	if (library.file_type_ == FASTQ) {
		getNextRead = &ReadLibrary::getNextFromFastq;
		if(library_){
			getline(library_, next_meta_);
		}
	} else if (library.file_type_ == FASTA) {
		getNextRead = &ReadLibrary::getNextFromFasta;
		while(library_ && (next_meta_.size() == 0 || next_meta_[0] != '>')){
			getline(library_, next_meta_);
		}
	} else {
		exit(1);
	}
}



std::vector<Read> ReadLibrary::getReadBatch(int const &batch_size){
	std::vector<Read> batch;
	for(int i = 0; i < batch_size && library_; ++i){
		std::string meta;
		std::string seq;
		(this->*getNextRead)(meta, seq);
		meta = meta.substr(1);
		Read read(meta, seq);
		batch.push_back(read);
	}
	return batch;
}
