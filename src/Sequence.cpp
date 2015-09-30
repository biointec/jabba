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
#include "Sequence.hpp"

#include <iostream>

twobit Sequence::encode(std::string const &decoded) {
	twobit encoded;
	for (int i = 0; i < decoded.size(); ++i) {
		encoded.push_back( decoded[i] == 'G' || decoded[i] == 'T' );
		encoded.push_back( decoded[i] == 'C' || decoded[i] == 'T' );
	}
	return encoded;
}

std::string Sequence::decode(twobit const &encoded) {
	std::string decoded;
	for(int i = 0; i < encoded.size(); i += 2) {
		if ( !encoded[i] ) {
			if ( !encoded[i + 1] ) {
				decoded += 'A';
		} else {
				decoded += 'C';
			}
		} else {
			if ( !encoded[i + 1] ) {
				decoded += 'G';
			} else {
				decoded += 'T';
			}
		}
	}
	return decoded;
}

twobit Sequence::reverseComplement(twobit const &encoded) {
	twobit reverse;
	for(int i = encoded.size() - 2; i > -1; i -= 2){
		if ( !encoded[i] ) {
				reverse.push_back(1);
			if ( !encoded[i + 1] ) {
				reverse.push_back(1);
			} else {
				reverse.push_back(0);
			}
		} else {
			reverse.push_back(0);
			if ( !encoded[i + 1] ) {
				reverse.push_back(1);
			} else {
				reverse.push_back(0);
			}
		}
	}
	return reverse;
}

std::string Sequence::reverseComplement(std::string const &sequence) {
	std::string reverse = "";
	
	std::string alphabet	= "ATGCKMRYSWBVHDNX.";
	std::string complement	= "TACGMKYRSWVBDHNX.";
	for(int i = sequence.length() - 1; i > -1; --i){
		reverse += complement.at(alphabet.find(sequence[i]));
	}
	return reverse;
}

std::string Sequence::collapse_to_acgt(std::string const &seq) {
	std::string collapsed;
	std::string to_A = "AaMmRrWwDdHhVvNnXx.";
	std::string to_C = "CcSsYyBb";
	std::string to_G = "GgKk";
	std::string to_T = "Tt";
	for (int i = 0; i < seq.size(); ++i) {
		if (to_A.find(seq[i]) != std::string::npos) {
			collapsed += 'A';
		} else if (to_C.find(seq[i]) != std::string::npos) {
			collapsed += 'C';
		} else if (to_G.find(seq[i]) != std::string::npos) {
			collapsed += 'G';
		} else if (to_T.find(seq[i]) != std::string::npos) {
			collapsed += 'T';
		} else {
			std::cerr << "unknown base: " << seq[i] << std::endl;
		}
	}
	return collapsed;
}


Sequence::Sequence(std::string const &seq) {
	set_sequence(seq);
}

void Sequence::set_sequence(std::string const &seq) {
	seq_ = encode(collapse_to_acgt(seq));
	size_ = seq.size();
}

