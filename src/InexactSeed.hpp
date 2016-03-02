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
#ifndef INEXACTSEED_HPP
#define INEXACTSEED_HPP

#include <vector>

#include "Seed.hpp"

class InexactSeed{
private:
        std::vector<Seed> seeds_; //list of exact seeds that make this inexact seed
public:
        /*
         *        ctors
         */
        InexactSeed(std::vector<Seed> seeds) : seeds_(seeds){}
        /*
         *        getters
         */
        std::vector<Seed> get_seeds() const {return seeds_;}
        int get_node() const {return seeds_[0].get_node();}
        int get_node_start() const {return seeds_[0].get_ref_start();}
        int get_node_end() const {return seeds_.back().get_ref_end();}
        int get_read_start() const {return seeds_[0].get_read_start();}
        int get_read_end() const {return seeds_.back().get_read_end();}
        /*
         *        methods
         */
        bool operator<(InexactSeed const &other) const {
                return get_read_start() < other.get_read_start();
        }
};

#endif
