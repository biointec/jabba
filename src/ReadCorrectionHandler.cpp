#include "ReadCorrectionHandler.hpp"

void ReadCorrectionHandler::workerThread(size_t myID, LibraryContainer& libraries)
{
        ReadCorrection readCorrection(graph_, settings_);
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        // local storage of reads
        std::vector<ReadRecord> myReadBuf;
        while (true) {
                size_t blockID, recordID;
                bool result = libraries.getRecordChunk(myReadBuf, blockID, recordID);
                readCorrection.correctChunk(myReadBuf);
                if (result)
                        libraries.commitRecordChunk(myReadBuf, blockID, recordID);
                else
                        break;
        }
}


void ReadCorrectionHandler::doErrorCorrection(LibraryContainer& libraries)
{
        const unsigned int& numThreads = settings_.get_num_threads();
        std::cout << "Number of threads: " << numThreads << std::endl;

        libraries.startIOThreads(settings_.get_thread_work_size(),
                                 10 * settings_.get_thread_work_size() * settings_.get_num_threads(),
                                 true);
        // start worker threads
        std::vector<std::thread> workerThreads(numThreads);
        for (size_t i = 0; i < workerThreads.size(); i++)
                workerThreads[i] = std::thread(&ReadCorrectionHandler::workerThread,
                                          this, i, std::ref(libraries));
        std::cout << "Worker threads started." << std::endl;
        // wait for worker threads to finish
        for_each(workerThreads.begin(), workerThreads.end(), std::mem_fn(&std::thread::join));

        libraries.joinIOThreads();
}

ReadCorrectionHandler::ReadCorrectionHandler(Graph& g, const Settings& s) :
        graph_(g), settings_(s)
{
        Util::startChrono();
}
ReadCorrectionHandler::~ReadCorrectionHandler()
{
}
