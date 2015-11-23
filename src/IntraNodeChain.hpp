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
#ifndef INTRANODECHAIN_HPP
#define INTRANODECHAIN_HPP

#include <string>

#include "InexactSeed.hpp"

class Read;
class Settings;

class IntraNodeChain{
	private:
		Settings const &settings_; //reference to settings
		std::vector<Seed> &seeds_; //reference to seeds
		int maximal_coverage_; //current highest coverage of node by a chain
		double min_cov_coef_; //used to determine required exact coverage
		/*
		 *	methods
		 */
		//check if seed fits
		bool checkFit(Seed const &prev, Seed const &next);
		bool checkFit(int const &ref_dist, int const &read_dist);
		//try to fit all seeds and extend a chain
		void fitAndExtend(std::vector<std::vector<Seed>> &consecutive_seeds,
			std::vector<int> &chain_coverage, Seed const &seed);
	public:
		/*
		 *	ctors
		 */
		IntraNodeChain(std::vector<Seed> &seeds,
			Settings const &settings);
		/*
		 *	methods
		 */
		//group the seeds in a node
		void groupSeedsInNode(std::vector<InexactSeed> &inexact_seeds);
};

#endif
