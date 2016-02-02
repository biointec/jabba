/***************************************************************************
 *   Copyright (C) 2015 Jan Fostier (jan.fostier@intec.ugent.be)           *
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

#ifndef READCORRECTIONHANDLER_H
#define READCORRECTIONHANDLER_H

#include "library.h"
#include "Graph.hpp"
#include "util.h"
// ============================================================================
// READ CORRECTION CLASS
// ============================================================================

class ReadCorrection
{
private:
        const Graph &graph_;
        const Settings &settings_;

        /**
         * Correct a specific read record
         * @param TODO
         */
        void correctRead(std::string& read);

        /**
         * Correct a specific read record
         * @param record Record to correct (input/output)
         */
        void correctRead(ReadRecord& record);

        /**
         * Find the node position pairs for a read using EssaMEM
         * @param read Reference to the read
         * @param seeds TODO
         */
        void findSeedMEM(const std::string& read, std::vector<Seed>& seeds);

        int correctRead(const std::string& read,
                        std::string& bestCorrectedRead,
                        const std::vector<Seed>& seeds);

public:
        /**
         * Default constructor
         * @param graph_ Reference to the De Bruijn graph
         * @param settings_ Reference to the settings class
         */
        ReadCorrection(const Graph& graph, const Settings& settings) :
                          graph_(graph), settings_(settings){}
        /**
         * Correct the records in one chunk
         * @param readChunk Chunk of records to correct
         */
        void correctChunk(std::vector<ReadRecord>& readChunk);
};

// ============================================================================
// READ CORRECTION HANDLER CLASS
// ============================================================================

class ReadCorrectionHandler
{
private:
        Graph &graph_;
        const Settings &settings_;

        /**
         * Entry routine for worker thread
         * @param myID Unique threadID
         * @param libaries Library container with libraries to be corrected
         */
        void workerThread(size_t myID, LibraryContainer& libraries);

public:
        /**
         * Default constructor
         */
        ReadCorrectionHandler(Graph &graph, const Settings &settings);

        /**
         * Destructor
         */
        ~ReadCorrectionHandler();

        /**
         * Perform error correction in the libaries
         * @param libraries Library container with libraries to be corrected
         */
        void doErrorCorrection(LibraryContainer &libraries);
};

#endif
