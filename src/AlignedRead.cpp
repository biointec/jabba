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
#include "AlignedRead.hpp"

#include <algorithm>
#include <iostream>

#include "Read.hpp"
#include "Graph.hpp"

AlignedRead::AlignedRead(Read &read, OutputMode output_mode)
      :        read_(read),
        output_mode_(output_mode)
{}

std::vector<std::pair<int, int>> AlignedRead::not_corrected() {
        sortAlongRead();
        std::vector<std::pair<int, int>> nc;
        int curr_pos = 0;
        int curr_al = 0;
        while (curr_pos < read_.size()) {
                int next_pos;
                if (curr_al < local_alignments_.size()) {
                        next_pos = local_alignments_[curr_al].get_read_start();
                } else {
                        next_pos = read_.size();
                }
                if (curr_pos < next_pos) {
                        nc.push_back(std::pair<int, int> (curr_pos, next_pos));
                        curr_pos = next_pos;
                }
                if (curr_al < local_alignments_.size()) {
                        curr_pos = local_alignments_[curr_al].get_read_end();
                }
                ++curr_al;
        }
        return nc;
}

void AlignedRead::correct(std::vector<LocalAlignment> const &als) {
        for (int i = 0; i < als.size(); ++i) {
                correct(als[i]);
        }
}

void AlignedRead::correct(LocalAlignment const &al) {
        local_alignments_.push_back(al);
}

void AlignedRead::sortAlongRead() {
        auto sort_alignments =
                [&](LocalAlignment a, LocalAlignment b)-> bool
        {
                return a.get_read_start() < b.get_read_start();
        };
        std::sort(local_alignments_.begin(), local_alignments_.end(), sort_alignments);
}

void AlignedRead::sortReadCov() {
        auto sort_alignments =
                [&](LocalAlignment a, LocalAlignment b)-> bool
        {
                return a.get_read_end() - a.get_read_start() < b.get_read_end() - b.get_read_start();
        };
        std::sort(local_alignments_.begin(), local_alignments_.end(), sort_alignments);
}

bool AlignedRead::fit(std::vector<int>::iterator const &index,
        std::vector<int> const &lai, std::vector<int> const & laj)
{
        if (index != laj.end()) {
                int d = index - laj.begin() + 1;
                d = d < lai.size() ? d : lai.size();
                for (int k = 1; k < d + 1; ++k) {
                        if (lai[lai.size() - k] != laj[d - k]) {
                                return false;
                        }
                }
                return true;
        } else {
                return false;
        }
}

bool AlignedRead::merge() {
        sortAlongRead();
        for (int i = 0; i < local_alignments_.size(); ++i) {
                std::vector<int> lai = local_alignments_[i].get_path();
                for (int j = 0; j < local_alignments_.size(); ++j) {
                        if (i == j) {
                                continue;
                        }
                        std::vector<int> laj = local_alignments_[j].get_path();
                        std::vector<int>::iterator index;
                        index = std::find(laj.begin(), laj.end(), lai.back());
                        if (fit(index, lai, laj)) {
                                lai.insert(lai.end(), index + 1, laj.end());
                                local_alignments_[i].set_path(lai);
                                int read_start = local_alignments_[i].get_read_start();
                                int read_end = local_alignments_[j].get_read_end();
                                int ref_start = local_alignments_[i].get_ref_start();
                                int ref_size = read_end - read_start;
                                if (ref_size < 0) {
                                        break;
                                }
                                local_alignments_[i].set_read_end(read_end);
                                local_alignments_[i].set_ref_end(ref_start + ref_size);
                                local_alignments_.erase(local_alignments_.begin() + j);
                                return true;
                        }
                }
        }
        return false;
}

void AlignedRead::print() {
        for (const auto &la : local_alignments_) {
                std::cout << la.to_string(read_.get_id()) << std::endl;
        }
}

void AlignedRead::getCorrectedRead(Graph const &graph, std::vector<std::string> &corrections) {
        while(merge());
        std::string corrected_read = "";
        if (local_alignments_.size() == 0) {
                return;
        }
        int max_cov = 0;
        sortReadCov();
        std::vector<LocalAlignment> corrs;
        for (const LocalAlignment &la : local_alignments_) {
                bool overlaps = false;
                for (const LocalAlignment &c : corrs) {
                        if(la.overlapsOnRead(c)) {
                                overlaps = true;
                                continue;
                        }
                }
                if(!overlaps) {
                        corrs.push_back(la);
                }
        }
        for (const LocalAlignment &la : corrs) {
                std::string result = graph.concatenateNodes(la.get_path());
                if (output_mode_ == LONG) {
                        corrections.push_back(result);
                } else if (output_mode_ == SHORT){
                        result = result.substr(la.get_ref_start(), la.get_ref_end() - la.get_ref_start());
                        corrections.push_back(result);
                }
        }
}
