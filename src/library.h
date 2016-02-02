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

#ifndef READLIBRARY_H
#define READLIBRARY_H

#include "readfile/readfile.h"
#include "util.h"       // timing routines

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>

#include <mutex>
#include <condition_variable>


#include <cassert>
#ifdef _MSC_VER
#include "msstdint.h"     // uint64_t types, etc.
#else
#include <stdint.h>
#endif
#include <thread>

// ============================================================================
// FILETYPE ENUM
// ============================================================================

typedef enum { FASTQ, FASTA, FASTQ_GZ, FASTA_GZ,
               SAM, SAM_GZ, RAW, RAW_GZ, UNKNOWN_FT} FileType;

std::ostream &operator<<(std::ostream &out, const FileType &fileType);

// ============================================================================
// READ LIBRARY CLASS
// ============================================================================

class ReadLibrary
{
private:
        std::string inputFilename;      // name of the input file
        std::string outputFilename;     // name of the output file

        FileType fileType;              // file type (FASTQ, FASTA, etc.)

        size_t numReads;                // number of reads in library
        double avgReadLength;           // average size of the reads

public:
        /**
         * Default constructor
         * @param inputFilename Input filename
         * @param outputFilename Output filename
         */
        ReadLibrary(const std::string& inputFilename,
                    const std::string& outputFilename);
        
        ReadLibrary(){};

        /**
         * Allocate and return the correct read file for a certain input
         * @return An allocated readfile
         */
        ReadFile* allocateReadFile() const;

        /**
         * Get the input filename
         * @return The input filename
         */
        std::string getInputFilename() const {
                return inputFilename;
        }

        /**
         * Get the output filename
         * @return The output filename
         */
        std::string getOutputFileName(){
                return outputFilename;
        }

        /**
         * Get the file type
         * @return The filetype
         */
        FileType getFileType() const {
                return fileType;
        }

        /**
         * Set the number of reads in this library
         * @return The number of reads in this library
         */
        void setNumReads(size_t target) {
                numReads = target;
        }

        /**
         * Get the number of reads in this library
         * @return The number of reads in this library
         */
        size_t getNumReads() const {
                return numReads;
        }

        /**
         * Set the read length
         * @param target The read length
         */
        void setReadLength(double target) {
                avgReadLength = target;
        }

        /**
         * Get the read length
         * @return The read length
         */
        double getReadLength() const {
                return avgReadLength;
        }
};

// ============================================================================
// LIBRARYCONTAINER CLASS
// ============================================================================

class BlockID
{
public:
        /**
         * Default constructor
         */
        BlockID() : fileID(0), blockID(0), numChunks(0), numRecords(0) {}

        /**
         * Default constructor
         */
        BlockID(size_t fileID_, size_t blockID_, size_t numChunks_,
                size_t numRecords_) : fileID(fileID_), blockID(blockID_),
                numChunks(numChunks_), numRecords(numRecords_) {}

        size_t fileID;          // file identifier to which this block belongs
        size_t blockID;         // unique block identifier within this file
        size_t numChunks;       // number of chunks within this block
        size_t numRecords;      // number of records within this block
};

class LibraryContainer
{
private:
        std::vector<ReadLibrary> container;             // all library files

        size_t targetBlockSize;                         // a block is a number of (overlapping) k-mers read in a single run
        size_t targetChunkSize;                         // a chunk is a piece of work attributed to one thread

        std::queue<BlockID> blockQueue;                 // queue holding all blocks to be processed

        // variables for thread-safe reading
        std::thread iThread;                            // input thread
        bool inputThreadWorking;                        // input thread still working
        std::mutex inputMutex;                          // input thread mutex
        std::condition_variable readBufReady;           // read buffer full condition
        std::condition_variable readBufEmpty;           // read buffer empty condition
        std::vector<ReadRecord> *actReadBuffer;         // active read buffer
        std::vector<ReadRecord> *idlReadBuffer;         // idle read buffer
        size_t actReadFileID;                           // identifier of the active read file
        size_t actReadBlockID;                          // identifier of the active read block ID
        size_t actReadBlockOffset;                      // offset within the active read block

        // variables for thread-safe writing
        std::thread oThread;                            // output thread
        std::mutex outputMutex;                         // output thread mutex
        std::condition_variable writeBufReady;          // write buffer ready condition
        std::condition_variable writeBufFull;           // write buffer full condition
        std::vector<ReadRecord> *actOutputBuffer;       // active write buffer
        std::vector<ReadRecord> *idlOutputBuffer;       // idle write buffer
        size_t actWriteFileID;                          // identifier of the output file
        size_t actWriteBlockID;                         // identifier of the output block
        size_t actWriteChunksLeft;                      // number of chunks left in active write block

        /**
         * Write the inputs for a specific library
         * @param library Reference to a specific library
         */
        void inputThreadLibrary(ReadLibrary& library);

        /**
         * Write the outputs for a specific library
         * @param library Reference to a specific library
         */
        void outputThreadLibrary(ReadLibrary& library);

        /**
         * Entry routine for the input threads
         */
        void inputThreadEntry();

        /**
         * Entry routine for the output threads
         */
        void outputThreadEntry();

public:
        /**
         * Add a read library to the container
         * @arg library Library to add
         */
        void insert(const ReadLibrary& library) {
                container.push_back(library);
        }

        /**
         * Get the number of inputs
         * @return The number of inputs
         */
        size_t getSize() const {
                return container.size();
        }

        /**
         * Get a specified input
         * @param index Input identifier
         * @return A reference to the input
         */
        ReadLibrary& getInput(size_t index) {
                assert(index < container.size());
                return container[index];
        }

        /**
         * Get a specified input
         * @param index Input identifier
         * @return A reference to the input
         */
        const ReadLibrary& getInput(size_t index) const {
                assert(index < container.size());
                return container[index];
        }

        /**
         * Get next record chunk from the input
         * @param buffer Buffer in which to store the records (output)
         * @param blockID Block identifier (output)
         * @param recordOffset Record offset within the block (output)
         * @return False if no more reads are available, true otherwise
         */
        bool getRecordChunk(std::vector<ReadRecord>& buffer,
                            size_t& blockID, size_t& recordOffset);

        /**
         * Get next read chunk from the input
         * @param buffer Buffer in which to store the reads (output)
         * @param blockID Block identifier (output)
         * @param recordOffset Record offset within the block (output)
         * @return False if no more reads are available, true otherwise
         */
        bool getReadChunk(std::vector<std::string>& buffer,
                          size_t& blockID, size_t& recordOffset);

        /**
         * Commit a ReadRecord chunk to the write buffer
         * @param blockID Block identfier obtained from get...Chunk()
         * @param recordOffset Record offset obtained from get...Chunk()
         */
        void commitRecordChunk(const std::vector<ReadRecord>& buffer,
                               size_t blockID,
                               size_t recordOffset);

        /**
         * Initialize read threading
         * @param targetChunkSize Target size for a single chunk
         * @param targetBlockSize Target size for a single block
         * @param writeReads True if the input reads are again to be written
         */
        void startIOThreads(size_t targetChunkSize,
                            size_t targetBlockSize,
                            bool writeReads = false);

        /**
         * Finalize read threading
         */
        void joinIOThreads();

        double getReadLength() const {
                size_t readLengthAvg = 0;
                size_t totalNumOfReads = 0;
                for (auto it : container){
                        readLengthAvg=((readLengthAvg *totalNumOfReads)+it.getReadLength()*it.getNumReads())/(totalNumOfReads+it.getNumReads());
                        totalNumOfReads=totalNumOfReads+it.getNumReads();
                }
                return readLengthAvg;

        }
};

#endif
