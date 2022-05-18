#include "vboworker.h"

VBOWorker::VBOWorker(QMutex* mutex, Chunk* chunk, std::vector<ChunkData>* vboChunks)
          :mutexVBO(mutex), chunk(chunk), vboChunks(vboChunks){}

void VBOWorker::run(){
    ChunkData chunkData(chunk);
    chunk->createVBOdata(&chunkData);
    mutexVBO->lock();
    vboChunks->push_back(chunkData);
    mutexVBO->unlock();
}
