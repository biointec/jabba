#include "ReadCorrectionHandler.hpp"
#include "Alignment.hpp"
#include "InterNodeChain.hpp"
#include "Read.hpp"
#include "AlignedRead.hpp"
#include "Seed.hpp"

void ReadCorrection::correctRead(ReadRecord& record)
{
        Read read(record.preRead, record.getRead());
        read.set_id(0);
        Alignment alignment(250, 30, 1, -4, -2, -3);
        InterNodeChain iernc(read, graph_, settings_, alignment);
        AlignedRead ar(read, settings_.get_output_mode());
        record.correction = iernc.chainSeeds(ar);
}

void ReadCorrection::correctChunk(vector<ReadRecord>& readChunk)
{
        for (auto& it : readChunk)
                correctRead(it);
}
