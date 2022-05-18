#pragma once

#include "misc.h"
#include "terrain.h"
#include <QMutex>
#include <QRunnable>
#include "smartpointerhelp.h"

class Chunk;

struct ChunkData {
    Chunk* chunk;

    std::vector<GLuint> transIdx;
    std::vector<glm::vec4> trans;

    std::vector<GLuint> opaqIdx;
    std::vector<glm::vec4> opaq;

    ChunkData(Chunk* chunk) : chunk(chunk), transIdx{}, trans{}, opaqIdx{}, opaq{}
    {}

};

class VBOWorker : public QRunnable{
protected:
    QMutex *mutexVBO;
    Chunk* chunk;
    std::vector<ChunkData>* vboChunks;
public:
    VBOWorker(QMutex* mutex, Chunk* chunk, std::vector<ChunkData>* vboChunks);
    void run() override;
};

