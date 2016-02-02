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

#include "library.h"

#include "tkmer.h"

#include "readfile/fastafile.h"
#include "readfile/fastqfile.h"
#include "readfile/rawfile.h"
#include "readfile/sequencefile.h"
#include "readfile/samfile.h"

#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

// ============================================================================
// FILETYPE ENUM
// ============================================================================

std::ostream &operator<<(std::ostream &out, const FileType &fileType) {
        switch (fileType) {
                case FASTA :
                        out << "fasta";
                        break;
                case FASTA_GZ :
                        out << "fasta.gz";
                        break;
                case FASTQ :
                        out << "fastq";
                        break;
                case FASTQ_GZ :
                        out << "fastq.gz";
                        break;
                case SAM:
                        out << "sam";
                        break;
                case SAM_GZ:
                        out << "sam.gz";
                        break;
                case RAW:
                        out << "raw";
                        break;
                case RAW_GZ:
                        out << "raw.gz";
                        break;
                case UNKNOWN_FT:
                        out << "unknown";
                        break;
        }

        return out;
}

// ============================================================================
// READLIBRARY CLASS
// ============================================================================

ReadLibrary::ReadLibrary(const std::string& inputFilename_,
                         const std::string& outputFilename_) :
        inputFilename(inputFilename_), outputFilename(outputFilename_),
        fileType(UNKNOWN_FT), numReads(0), avgReadLength(0.0)
{
        // try to figure out the file format based on the extension
        string extension, baseFilename;

        if (inputFilename.length() >= 4)
                extension = inputFilename.substr(inputFilename.length() - 4);
        transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
        if (extension == ".SAM") {
                fileType = SAM;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 4);
        } else if (extension == ".RAW") {
                fileType = RAW;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 4);
        }

        if (inputFilename.length() >= 6)
                extension = inputFilename.substr(inputFilename.length() - 6);
        transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
        if (extension == ".FASTA") {
                fileType = FASTA;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 6);
        } else if (extension == ".FASTQ") {
                fileType = FASTQ;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 6);
        }

        if (inputFilename.length() >= 7)
                extension = inputFilename.substr(inputFilename.length() - 7);
        transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
        if (extension == ".RAW.GZ") {
                fileType = RAW_GZ;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 7);
        } else if (extension == ".SAM.GZ") {
                fileType = SAM_GZ;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 7);
        }

        if (inputFilename.length() >= 9)
                extension = inputFilename.substr(inputFilename.length() - 9);
        transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
        if (extension == ".FASTA.GZ") {
                fileType = FASTA_GZ;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 9);
        } else if (extension == ".FASTQ.GZ") {
                fileType = FASTQ_GZ;
                baseFilename = inputFilename.substr(0, inputFilename.length() - 9);
        }

        if (fileType == UNKNOWN_FT) {
                cerr << "Jabba can not open the following file: '" << inputFilename << "'\n";
                cerr << "Expected one of the following extensions: .fastq, .fasta, .sam, .raw (or .gz variants thereof)\n";
                exit(EXIT_FAILURE);
        }

        if (!Util::fileExists(inputFilename)) {
                cerr << "Jabba can not open the following file: '" << inputFilename << "'\n";
                cerr << "Exiting... " << endl;
                exit(EXIT_FAILURE);
        }

        // set the outputFilename only if not specified by the user
        if (outputFilename.empty()) {
                ostringstream oss;
                oss << baseFilename << ".corr." << fileType;
                outputFilename = oss.str();
        }
}

ReadFile* ReadLibrary::allocateReadFile() const
{
        switch (getFileType()) {
                case FASTA :
                        return new FastAFile(false);
                case FASTA_GZ :
                        return new FastAFile(true);
                case FASTQ :
                        return new FastQFile(false);
                case FASTQ_GZ :
                        return new FastQFile(true);
                case SAM :
                        return new SamFile(false);
                case SAM_GZ :
                        return new SamFile(true);
                case RAW :
                        return new RawFile(false);
                case RAW_GZ :
                        return new RawFile(true);
                case UNKNOWN_FT:
                        assert(false);
                        return NULL;
        }

        assert ( false );
        return NULL;
}

// ============================================================================
// LIBRARY CONTAINER CLASS
// ============================================================================

bool LibraryContainer::getRecordChunk(vector<ReadRecord>& buffer,
                                      size_t& blockID, size_t& recordOffset)
{
        // clear the buffer
        buffer.clear();
        // wait until reads become available
        std::unique_lock<std::mutex> lock(inputMutex);
        readBufReady.wait(lock, [this]{return ((actReadBlockOffset < actReadBuffer->size()) ||
                                               (!inputThreadWorking)); });
        blockID = actReadBlockID;
        recordOffset = actReadBlockOffset;

        // find out how many records to copy
        size_t thisChunkSize = 0;
        for (size_t i = recordOffset; i < actReadBuffer->size(); i++) {
                actReadBlockOffset++;
                thisChunkSize += (*actReadBuffer)[i].getReadLength() + 1 - Kmer::getK();
                if (thisChunkSize >= targetChunkSize)
                        break;
        }
        // actually copy records
        buffer = vector<ReadRecord>(actReadBuffer->begin() + recordOffset,
                                    actReadBuffer->begin() + actReadBlockOffset);
        // if you took the final block of reads, notify the input thread
        if (actReadBlockOffset >= actReadBuffer->size())
                readBufEmpty.notify_one();

        lock.unlock();
        return !buffer.empty();
}

bool LibraryContainer::getReadChunk(vector<string>& buffer,
                                    size_t& blockID, size_t& recordOffset)
{
        // clear the buffer
        buffer.clear();

        // wait until reads become available
        std::unique_lock<std::mutex> lock(inputMutex);
        readBufReady.wait(lock, [this]{return ((actReadBlockOffset < actReadBuffer->size()) ||
                                               (!inputThreadWorking)); });

        blockID = actReadBlockID;
        recordOffset = actReadBlockOffset;

        // find out how many records to copy
        size_t thisChunkSize = 0;
        for (size_t i = recordOffset; i < actReadBuffer->size(); i++) {
                actReadBlockOffset++;
                thisChunkSize += (*actReadBuffer)[i].getReadLength() + 1 - Kmer::getK();
                buffer.push_back((*actReadBuffer)[i].getRead());
                if (thisChunkSize >= targetChunkSize)
                        break;
        }

        // if you took the final block of reads, notify the input thread
        if (actReadBlockOffset >= actReadBuffer->size())
                readBufEmpty.notify_one();

        lock.unlock();

        return !buffer.empty();
}


void LibraryContainer::inputThreadLibrary(ReadLibrary& input)
{
        // read counters
        size_t totNumReads = 0, numTooShort = 0, totReadLength = 0;

        // open the read file
        ReadFile *readFile = input.allocateReadFile();
        readFile->open(input.getInputFilename());

        // aux variables
        ReadRecord record;

        while (true) {
                // fill up the idle read buffer (only this thread has access)
                idlReadBuffer->clear();
                size_t thisBlockSize = 0, thisBlockNumRecords = 0;
                size_t thisBlockNumChunks = 0, thisChunkSize = 0;
                while ((thisBlockSize < targetBlockSize) &&
                        readFile->getNextRecord(record)) {
                        thisBlockNumRecords++;
                        totNumReads++;
                        totReadLength += record.getReadLength();

                        // if the read is short than k, no need to process it
                        /*if (record.getReadLength() < Kmer::getK()) {
                                numTooShort++;
                                continue;
                        }*/

                        idlReadBuffer->push_back(record);
                        thisBlockSize += record.getReadLength() + 1 - Kmer::getK();

                        // count the number of chunks in this block
                        thisChunkSize += record.getReadLength() + 1 - Kmer::getK();
                        if (thisChunkSize >= targetChunkSize) {
                                thisBlockNumChunks++;
                                thisChunkSize = 0;
                        }
                }

                // also count the final chunk
                if (thisChunkSize > 0)
                        thisBlockNumChunks++;

                cout << "Number of reads processed: " << totNumReads << "\r"; cout.flush();

                // wait until active buffer is empty
                std::unique_lock<std::mutex> lock(inputMutex);
                readBufEmpty.wait(lock, [this]{return actReadBlockOffset >= actReadBuffer->size();});

                actReadBuffer->clear();
                actReadBlockOffset = 0;
                actReadBlockID++;

                // swap active buffer and idle buffer
                std::swap(actReadBuffer, idlReadBuffer);

                BlockID blockID(actReadFileID, actReadBlockID, thisBlockNumChunks, thisBlockNumRecords);
                blockQueue.push(blockID);

                // notify workers that more work is available
                readBufReady.notify_all();
                lock.unlock();

                // file has completely been read
                if (!readFile->good())
                        break;
        }

        cout << "Number of reads processed: " << totNumReads << endl;
        cout << "Average read length: " << totReadLength/totNumReads << endl;
        input.setReadLength(totReadLength/totNumReads);
        input.setNumReads(totNumReads);

        readFile->close();
        // free temporary memory
        delete readFile;

        // issue a warning about skipped reads
        if (numTooShort > 0) {
                double perc = 100.0 * (double)numTooShort/(double)totNumReads;
                streamsize prev = cout.precision (2);

                cout << "\n\tWARNING: Skipped " << numTooShort << " reads ("
                     << perc << "% of total) because they were shorter than "
                     << "the kmer size" << endl;

                cout.precision(prev);
        }
}

void LibraryContainer::commitRecordChunk(const vector<ReadRecord>& buffer,
                                         size_t blockID, size_t recordOffset)
{
        // wait until write becomes available
        std::unique_lock<std::mutex> lock(outputMutex);
        writeBufReady.wait(lock, [this, blockID]{return blockID == actWriteBlockID;});

        copy(buffer.begin(), buffer.end(), actOutputBuffer->begin() + recordOffset);

        actWriteChunksLeft--;
        if (actWriteChunksLeft == 0)
                writeBufFull.notify_one();

        lock.unlock();
}

void LibraryContainer::outputThreadLibrary(ReadLibrary& input)
{
        // open the read file
        ReadFile *readFile = input.allocateReadFile();
        readFile->open(input.getOutputFileName(), WRITE);

        while (true) {
                // wait until the input thread has read a new block
                std::unique_lock<std::mutex> ilock(inputMutex);
                readBufReady.wait(ilock, [this]{return ((!blockQueue.empty()) ||
                                                        (!inputThreadWorking)); });

                // if more work is available, get it from the queue
                bool moveToNextFile = true;
                BlockID blockID;
                if (!blockQueue.empty()) {
                        blockID = blockQueue.front();
                        blockQueue.pop();
                        if (blockID.fileID == actWriteFileID)
                                moveToNextFile = false;
                }

                ilock.unlock();

                // wait until active buffer is full
                std::unique_lock<std::mutex> lock(outputMutex);
                writeBufFull.wait(lock, [this]{return actWriteChunksLeft == 0;});

                // swap active buffer and idle buffer
                std::swap(actOutputBuffer, idlOutputBuffer);

                actWriteBlockID = blockID.blockID;
                actWriteChunksLeft = blockID.numChunks;
                actOutputBuffer->resize(blockID.numRecords);

                // notify workers that work can again be written
                writeBufReady.notify_all();
                lock.unlock();

                // write the idle read buffer (only this thread has access)
                for (size_t i = 0; i < idlOutputBuffer->size(); i++)
                        readFile->writeRecord((*idlOutputBuffer)[i]);

                if (!readFile->good())
                        throw ios_base::failure("Cannot write to " + input.getOutputFileName());

                idlOutputBuffer->clear();

                // if we need to start a new file: get out
                if (moveToNextFile)
                        break;
        }

        readFile->close();
        delete readFile;
}

void LibraryContainer::inputThreadEntry()
{
        // read all input data
        for (size_t i = 0; i < getSize(); i++) {
                ReadLibrary &input = getInput(i);

                cout << "Processing file " << i+1 << "/" << getSize() << ": "
                     << input.getInputFilename() << ", type: " << input.getFileType()
                     << endl;

                inputThreadLibrary(input);
                actReadFileID++;
        }

        // wait until active buffer is empty
        unique_lock<std::mutex> lock(inputMutex);
        inputThreadWorking = false;
        readBufReady.notify_all();
        lock.unlock();
}

void LibraryContainer::outputThreadEntry()
{
        // write all data
        for (size_t i = 0; i < getSize(); i++) {
                ReadLibrary &input = getInput(i);
                outputThreadLibrary(input);
                actWriteFileID++;
        }
}

void LibraryContainer::startIOThreads(size_t targetChunkSize_,
                                      size_t targetBlockSize_,
                                      bool writeReads)
{
        targetChunkSize = targetChunkSize_;
        targetBlockSize = targetBlockSize_;

        // input threads
        inputThreadWorking = true;
        actReadBuffer = new vector<ReadRecord>;
        idlReadBuffer = new vector<ReadRecord>;
        actReadFileID = actReadBlockID = actReadBlockOffset = 0;

        iThread = thread(&LibraryContainer::inputThreadEntry, this);

        if (!writeReads)
                return;

        actOutputBuffer = new vector<ReadRecord>;
        idlOutputBuffer = new vector<ReadRecord>;
        actWriteFileID = actWriteBlockID = actWriteChunksLeft = 0;
        oThread = thread(&LibraryContainer::outputThreadEntry, this);
}

void LibraryContainer::joinIOThreads()
{
        iThread.join();

        delete idlReadBuffer; idlReadBuffer = NULL;
        delete actReadBuffer; actReadBuffer = NULL;

        if (oThread.joinable()) {
                oThread.join();
                delete idlOutputBuffer; idlOutputBuffer = NULL;
                delete actOutputBuffer; actOutputBuffer = NULL;
        }

        // make sure the queue is empty
        std::queue<BlockID>().swap(blockQueue);
}
