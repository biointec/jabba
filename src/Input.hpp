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
#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>

typedef enum {FASTQ, FASTA, FASTQ_GZ, FASTA_GZ, SAM, SAM_GZ, BAM, RAW, RAW_GZ}
	FileType;
std::ostream &operator<<(std::ostream &out, const FileType &fileType);

struct Input {
	FileType file_type_;
	std::string basename_; //is required when file is in another folder
	std::string filename_;
	
	Input(FileType file_type, std::string basename)
	 :  file_type_(file_type),
		basename_(),
		filename_(basename)
	{
		basename_ = parseBasename(filename_);
	}
	static std::string parseBasename(std::string const &filename) {
		std::string basename;
		size_t location = filename.find_last_of('/');
		if(location != filename.npos) {
			//was found so get substring from there!
			basename = filename.substr(location + 1);
		} else {
			//not found, relative filename, just copy to basename
			basename = filename;
		}
		return basename;
	}
};

#endif
