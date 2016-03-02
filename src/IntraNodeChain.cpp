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
#include "IntraNodeChain.hpp"

#include <sstream>
#include <map>
#include <algorithm>
#include <iostream>

#include "Settings.hpp"

IntraNodeChain::IntraNodeChain(std::vector<Seed> &seeds,
        Settings const &settings)
 :        settings_(settings),
        seeds_(seeds),
        maximal_coverage_(0),
        min_cov_coef_(.15)
{}

bool IntraNodeChain::checkFit(Seed const &prev, Seed const &next) {
        return prev.get_ref_start() <= next.get_ref_start()
                && checkFit(next.get_ref_start() - prev.get_ref_end(),
                next.get_read_start() - prev.get_read_end());
}

bool IntraNodeChain::checkFit(int const &ref_dist, int const &read_dist) {
        if (ref_dist <= 0 && read_dist <= 0) {
                //seeds overlap in both ref and read, 
                //check the overlap and return if it can be fixed
                return (ref_dist > -20 && read_dist > -20);
        }
        //if seeds dont overlap in both ref and read, we check if the gaps 
        //aren't too large
        return (ref_dist * 0.9 - 10 < read_dist
                && read_dist < ref_dist * 1.2 + 20);
}

void IntraNodeChain::fitAndExtend(
                std::vector<std::vector<Seed>> &consecutive_seeds, 
                std::vector<int> &chain_coverage, Seed const &seed) {
        bool extends = false;
        for (int j = 0; j < consecutive_seeds.size(); ++j) {
                std::vector<Seed> chain = consecutive_seeds[j];
                int k = 0;
                //find the location in the chain where we can insert this seed
                while (k < chain.size()
                        && seed.get_read_start() > chain[k].get_read_start())
                {
                        ++k;
                }
                //check if the Seed fits
                bool fits = true;
                if (0 < k) {
                        Seed prev = chain[k - 1];
                        fits = fits && checkFit(prev, seed);
                }
                if (fits && k < chain.size()) {
                        Seed next = chain[k];
                        fits = fits && checkFit(seed, next);
                }
                if (fits) {
                        //if the Seed fits, place it
                        chain.insert(chain.begin() + k, seed);
                        consecutive_seeds[j] = chain;
                        chain_coverage[j] += seed.get_length();
                        maximal_coverage_ = maximal_coverage_ > chain_coverage[j]
                                ? maximal_coverage_ : chain_coverage[j];
                        extends = true;
                }
        }
        //add new chain if seed doesn't extend any chain and is sufficiently large
        if (!extends
                && ((int unsigned) seed.get_length() > maximal_coverage_ / 5))
        {
                std::vector<Seed> chain;
                chain.push_back(seed);
                consecutive_seeds.push_back(chain);
                chain_coverage.push_back(seed.get_length());
                maximal_coverage_ = maximal_coverage_ > seed.get_length()
                        ? maximal_coverage_ : seed.get_length();
        }
}

void IntraNodeChain::groupSeedsInNode(std::vector<InexactSeed> &inexact_seeds) {
        std::vector<std::vector<Seed>> consecutive_seeds;
        std::vector<int> chain_coverage;
        //sort seeds from largest to smallest
        auto compare_seed_size = [&](Seed a, Seed b)-> bool {
                return a.get_length() > b.get_length();
        };
        std::sort(seeds_.begin(), seeds_.end(), compare_seed_size);
        //loop through all seeds and check if they fit
        for (int i = 0; i < seeds_.size(); ++i) {
                fitAndExtend(consecutive_seeds, chain_coverage, seeds_[i]);
        }
        double mcc = min_cov_coef_;
        while (inexact_seeds.size() == 0 && consecutive_seeds.size() > 0) {
                for (int i = 0; i < consecutive_seeds.size(); ++i) {
                        int req_cov = (consecutive_seeds[i].back().get_ref_end()
                                - consecutive_seeds[i][0].get_ref_start())
                                * mcc;
                        InexactSeed is(consecutive_seeds[i]);
                        inexact_seeds.push_back(is);
                }
                mcc *= 0.9;
        }
}
