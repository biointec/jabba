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

#ifndef TIGHTSTRING_HPP
#define TIGHTSTRING_HPP

#include <iostream>
#include <fstream>
#include "Nucleotide.hpp"

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

class TString;

//============================================================================
// KMER ITERATOR
// ============================================================================

class TStringIt {

protected:
        const TString &tStr;    // tight string
        size_t offset;          // string offset

public:
        /**
         * Default constructor
         * @param tStr Tight string reference
         * @param offset Offset in the string
         */
        TStringIt(const TString &tStr, size_t offset);

        /**
         * Overloading of == operator
         * @return true of false
         */
        bool operator==(const TStringIt& it) {
                return (&tStr == &it.tStr) && (offset == it.offset);
        }

        /**
         * Overloading the != operator
         * @return true of false
         */
        bool operator!=(const TStringIt& it) {
                return !(*this == it);
        }

        /**
         * Overloading of postfix ++ operator
         * @return Copy of the iterator before the ++ operation
         */
        TStringIt operator++(int notused);

        /**
         * Overloading of prefix ++ operator
         * @return Reference to the iterator after ++ operator
         */
        TStringIt& operator++();

};

class RevTStringIt : public TStringIt {

public:
        /**
         * Default constructor
         * @param tStr Tight string reference
         * @param offset Offset in the string
         */
        RevTStringIt(const TString &tStr, size_t offset) :
                TStringIt(tStr, offset) {};

        /**
         * Overloading of postfix ++ operator
         * @return Copy of the iterator before the ++ operation
         */
        RevTStringIt operator++(int notused);

        /**
         * Overloading of prefix ++ operator
         * @return Reference to the iterator after ++ operator
         */
        RevTStringIt& operator++();
};

// ============================================================================
// TIGHTSTRING CLASS
// ============================================================================

class TString {

private:
        /**
         * Change byteID and byteOff to point to the next character
         * @param byteID Identifier for the byte (intput/output)
         * @param byteOff Identifier for the byte offset (intput/output)
         * @returns False if we're a the right-most position
         */
        bool gotoNextChar(size_t &byteID, size_t &byteOff) const;

        /**
         * Initialize byteID and byteOff to point to the left-most character
         * @param byteID Identifier for the byte (output)
         * @param byteOff Identifier for the byte offset (output)
         */
        void initOffsets(size_t &byteID, size_t &byteOff) const;

        static const uint8_t charToNucleotideLookup[4];
        static const char charMask;
        static const char nucleotideToCharLookup[4];
        static const uint8_t nucleotideMask;

        uint32_t length;        // number of nucleotides in the string
        uint8_t * buf;          // 2 bit encoding of sequence

public:
        typedef TStringIt iterator;
        typedef RevTStringIt reverse_iterator;

        /**
         * Default constructor
         */
        TString() : length(0), buf(NULL) {}

        /**
         * Constructor from an stl string
         * @param str String containing only 'A', 'C', 'G' and 'T'
         */
        TString(std::string str);

        /**
         * Destructor
         */
        ~TString() { delete [] buf; }

        /**
         * Create a tstring from an input file stream
         * @param ifs Opened input file stream
         */
        TString(std::ifstream &ifs);

        /**
         * Read a tstring from an input file stream
         * @param ifs Opened input file stream
         */
        void read(std::ifstream &ifs);

        /**
         * Set the sequence from an stl string
         * @param str String containing only 'A', 'C', 'G' and 'T'
         */
        void setSequence(const std::string &str);

        /**
         * Get the sequence and save as stl string
         * @return Stl string containing the sequence
         */
        std::string getSequence() const;

        /**
         * Get a subsequence of this node
         * @param offset Start offset
         * @param len Length of node
         * @return stl string containing the sequence
         */
        std::string substr(size_t offset, size_t len) const;

        /**
         * Complement the tight string
         */
        void complement();

        /**
         * Reverse a tight string
         */
        void reverse();

        /**
         * Reverse complement a tight string
         */
        void reverseComplement();

        /**
         * Get the length of the sequence
         * @return The length of the sequence
         */
        uint32_t getLength() const {
                return length;
        }

        /**
         * Append a tight string to the current one
         * @param tString String to be appended
         */
        void append(const TString &tString);

        /**
         * Clear the contents of the tight string
         */
        void clear() {
                delete [] buf;
                buf = NULL;
                length = 0;
        }

        /**
         * Operator [] overloading (read-only operation)
         * @param index Index
         */
        char operator[](int index) const;

        /**
         * Operator<< overloading
         * @param out Output stream to add string to
         * @param tString Right-hand side tight string
         * @return Output stream with the tstring added to it
         */
        friend std::ostream &operator<<(std::ostream &out,
                                        const TString &tString);

        /**
         * Get the leftmost nucleotide of this node
         * @return The leftmost nucleotide
         */
        char peekNucleotideLeft() const {
                return Nucleotide::nucleotideToChar(buf[0]);
        }

        /**
         * Get the rightmost nucleotide of this node
         * @return The rightmost nucleotide
         */
        char peekNucleotideRight() const {
                size_t byteID = (length-1) / 4;
                size_t bitID = 2*((length-1) % 4);
                return Nucleotide::nucleotideToChar(buf[byteID] >> bitID);
        }

        /**
         * Write a kmer to file
         * @param ofs Openen output file stream
         */
        void write(std::ofstream& ofs) const {
                // write all but the most significant byte
                ofs.write((char*)&length, sizeof(length));

                // write the most significant byte without metadata
                size_t numBytes = (length + 3) / 4;
                ofs.write((char*)buf, numBytes);
        }

};

#endif
