#ifndef BLOCKTYPEWORKER_H
#define BLOCKTYPEWORKER_H

#include "chunk.h"
#include "terrain.h"
#include <QMutex>
#include <QRunnable>
#include "smartpointerhelp.h"

class Chunk;
class BlockTypeWorker : public QRunnable{
protected:
    QMutex *mutexBlock;
    std::vector<Chunk*> chunk;
    std::vector<Chunk*>* chunksData;
    glm::vec2 zonePosition;
public:
    //    Terrain* terrain;
    BlockTypeWorker(QMutex *mutexBlock, std::vector<Chunk*> chunk, std::vector<Chunk*>* chunksData, glm::vec2 zonePosition);
    void run() override;

};

#endif // BLOCKTYPEWORKER_H
