/***************************************************************************
 *   Copyright (C) 2014, 2015 Jan Fostier (jan.fostier@intec.ugent.be)     *
 *   Copyright (C) 2014, 2015 Mahdi Heydari (mahdi.heydari@intec.ugent.be) *
 *   This file is part of Brownie                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Alignment.hpp"
#include <string>
#include <stdlib.h>
#include <algorithm>
using namespace std;

// ============================================================================
// ALIGNMENT CLASS
// ============================================================================

int Alignment::align(const string& s1, const string& s2)
{
        // reallocate memory if necessary
        int thisMaxDim = max(s1.length(), s2.length());
        if (thisMaxDim > maxDim) {
                maxDim = thisMaxDim;
                delete [] M;
                M = new int[(2*maxIndel+1) * (maxDim+1)];
        }

        // initialize the borders of the matrix
        for (int i = 0; i <= maxIndel; i++)
                (*this)(i, 0) = i * del;

        for (int i = 0; i <= maxIndel; i++)
                (*this)(0, i) = i * ins;

        // initialize the rest of the bulk of the matrix
        for (int i = 1; i <= (int)s1.length(); i++) {
                for (int j = max(1, i - maxIndel); j <= min((int)s2.length(), i + maxIndel); j++) {

                        bool hit = (s1[i-1] == s2[j-1]);
                        if ((s1[i-1] == 'N') || (s2[j-1] == 'N'))
                                hit = true;

                        int thisMatch = (*this)(i-1, j-1) + ((hit) ? match : mismatch);
                        int thisDel = (j < i + maxIndel) ? (*this)(i-1, j) + del : thisMatch-1;
                        int thisIns = (j > i - maxIndel) ? (*this)(i, j-1) + ins : thisMatch-1;

                        int score = max(thisMatch, max(thisDel, thisIns));
                        (*this)(i, j) = score;
                }
        }

        return (*this)(s1.length(), s2.length());
}

Alignment::Alignment(int maxDim_, int maxIndel_, int match_,
                           int mismatch_, int ins_, int del_) : maxDim(maxDim_),
                           maxIndel(maxIndel_), match(match_),
                           mismatch(mismatch_), ins(ins_), del(del_)
{
        M = new int[(2*maxIndel+1) * (maxDim+1)];
}

void Alignment::printMatrix() const
{
        for (int l = 0; l < 2*maxIndel+1; l++) {
                for (int k = 0; k < maxDim+1; k++)
                        cout << M[k * (2 * maxIndel + 1) + l] << "\t";
                cout << endl;
        }
}

void Alignment::printAlignment(const string& s1, const string& s2) const
{
        string al1, al2;

        int i = s1.size();
        int j = s2.size();
        while (i > 0 || j > 0) {
                bool hit = (s1[i-1] == s2[j-1]);
                if ((s1[i-1] == 'N') || (s2[j-1] == 'N'))
                        hit = true;

		if ((j < i + maxIndel) && ((*this)(i, j) == (*this)(i-1, j) + del)) {
			al1.push_back(s1[i-1]);
			al2.push_back('-');
			i--;
		} else if ((j < i + maxIndel) && ((*this)(i, j) == (*this)(i, j - 1) + ins)) {
			al1.push_back('-');
			al2.push_back(s2[j-1]);
			j--;
		} else if ((i > 0) && (j > 0) && ((*this)(i, j) == ((*this)(i-1, j-1) + ((hit) ? match : mismatch)))) {
			al1.push_back(s1[i-1]);
			al2.push_back(s2[j-1]);
			i--;
			j--;
		} else {
			/*if ((j < i + maxIndel) && ((*this)(i, j) == (*this)(i-1, j))) {
				al1.push_back(s1[i-1]);
				al2.push_back('-');
				i--;
			} else if ((j < i + maxIndel) && ((*this)(i, j) == (*this)(i, j - 1))) {
				al1.push_back('-');
				al2.push_back(s2[j-1]);
				j--;
			} else {
				*/std::cerr << i << " " << j << " " << s1[i-1] << " " << s2[j-1] << " " << (*this)(i-1, j-1) << " " << (*this)(i-1, j) << " " << (*this)(i, j-1) << " " << (*this)(i, j) << std::endl;
				std::cerr << "error in alignment\n";
				reverse(al1.begin(), al1.end());
        reverse(al2.begin(), al2.end());

        cerr << al1 << endl;
        for (int i = 0; i < max((int)al1.size(), (int)al2.size()); i++)
                if (al1[i] == al2[i] || al1[i] == 'N' || al2[i] == 'N')
                        cerr << "|";
                else
                        cerr << "*";
        cerr << "\n" << al2 << endl;
				exit(1);
			//}
		}
		
		/*
                if ((i > 0) && (j > 0) && ((*this)(i, j) == ((*this)(i-1, j-1) + ((hit) ? match : mismatch)))) {
                        al1.push_back(s1[i-1]);
                        al2.push_back(s2[j-1]);
                        i--;
                        j--;
                } else if ((j < i + maxIndel) && ((*this)(i, j) == (*this)(i-1, j) + gap)) {
                        al1.push_back(s1[i-1]);
                        al2.push_back('-');
                        i--;
                } else {
                        al1.push_back('-');
                        al2.push_back(s2[j-1]);
                        j--;
                }
                */
        }

        reverse(al1.begin(), al1.end());
        reverse(al2.begin(), al2.end());

        cout << al1 << endl;
        for (int i = 0; i < max((int)al1.size(), (int)al2.size()); i++)
                if (al1[i] == al2[i] || al1[i] == 'N' || al2[i] == 'N')
                        cout << "|";
                else
                        cout << "*";
        cout << "\n" << al2 << endl;
}
