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
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <vector>

#include "Input.hpp"

#ifdef _MSC_VER
        #include <windows.h>
#else
        #include "dirent.h"
        #include <sys/stat.h>
#endif

class Settings {
private:
	int num_threads_; //maximal number of threads
	std::string directory_; //output directory
	Input graph_; //graph file
	int dbg_k_; //de Bruijn graph k-mer size
	int essa_k_; //ESSA sparseness parameter
	int max_passes_; //maximal number of passes
	std::vector<Input> inputs_; //list of input files
	int min_len_; //minimal seed length
	/*
	 *	methods
	 */
	//print information about the program
	void printProgramInfo() const;
	//print usage instructions
	void printUsage() const;
	//log instructions to file
	void logInstructions(int argc, char **args) const;
public:
	/*
	 *	ctors
	 */
	Settings(int argc, char** args);
	/*
	 *	methods
	 */
	//gettres
	int get_num_threads() const {return num_threads_;}
	std::string get_directory() const {return directory_;}
	Input get_graph() const {return graph_;}
	int get_dbg_k() const {return dbg_k_;}
	int get_essa_k() const {return essa_k_;}
	int get_max_passes() const {return max_passes_;}
	std::vector<Input> get_input() const {return inputs_;}
	int get_min_len() const {return min_len_;}
	std::string getLogFilename() const;
};

#endif
