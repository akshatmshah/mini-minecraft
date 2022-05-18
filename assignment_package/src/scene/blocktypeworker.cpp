#include "blocktypeworker.h"

BlockTypeWorker::BlockTypeWorker(QMutex *mutexBlock, std::vector<Chunk*> chunk, std::vector<Chunk*>* chunksData, glm::vec2 zonePosition)
                : mutexBlock(mutexBlock), chunk(chunk), chunksData(chunksData), zonePosition(zonePosition){}

void BlockTypeWorker::run(){
    for(auto& c : chunk){
        c->CreateChunk(c->x, c->z);
//        mp_terrain->CreateChunk(c->x, c->z);
        c->generated = true;
        mutexBlock->lock();
        chunksData->push_back(c);
        mutexBlock->unlock();
    }
}
