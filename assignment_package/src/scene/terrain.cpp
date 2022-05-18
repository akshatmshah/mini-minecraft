#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include "noise.h"


Terrain::Terrain(OpenGLContext *context, const std::vector<uPtr<Model>> &models)
    : m_chunks(), m_generatedTerrain(), m_geomCube(context), mp_context(context), models(models)
{}

Terrain::~Terrain() {
    m_geomCube.destroyVBOdata();
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)){
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z, models);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = move(chunk);
//    cPtr->createVBOdata();
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
    return cPtr;
}

// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    //    m_geomCube.clearOffsetBuf();
    //    m_geomCube.clearColorBuf();

    std::vector<glm::vec3> offsets, colors;
    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if(hasChunkAt(x, z)){
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                //                if(chunk->elemCount() == -1){
                //                    chunk->createVBOdata();
                //                }
                if(chunk->elemCountOpq() != -1){
                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawOpq(*chunk);
                }
            }
        }
    }

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if(hasChunkAt(x, z)){
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                //                if(chunk->elemCount() == -1){
                //                    chunk->createVBOdata();
                //                }
                if(chunk->elemCountTrans() != -1){

                    shaderProgram->setModelMatrix(glm::translate(glm::mat4(), glm::vec3(x, 0, z)));
                    shaderProgram->drawTrans(*chunk);
                }
            }
        }
    }

    //    m_geomCube.createInstancedVBOdata(offsets, colors);
    //    shaderProgram->drawInstanced(m_geomCube);
}

QSet<int64_t> Terrain::getNeighborZones(glm::vec2 currZone){
    QSet<int64_t> output;
    for(int x = currZone.x - 128; x <= currZone.x + 128; x+= 64){
        for(int z = currZone.y - 128; z <= currZone.y + 128; z += 64){
            output.insert(toKey(x, z));
        }
    }
    return output;
}

void Terrain::generateTerrain(glm::vec3 currentPos, glm::vec3 prevPos){
    //    std::cout << currentPos.x << " curr " << currentPos.z << std::endl;
    //    std::cout << prevPos.x << " prev " << prevPos.z << std::endl;
    glm::vec2 zoneOrigin = glm::vec2(floor(currentPos.x / 64.f) * 64.f, floor(currentPos.z / 64.f) * 64.f);
    glm::vec2 prevZoneOrigin =  glm::vec2(floor(prevPos.x / 64.f) * 64.f, floor(prevPos.z / 64.f) * 64.f);

    auto zoneN = getNeighborZones(zoneOrigin);
    auto prevzoneN = getNeighborZones(prevZoneOrigin);

    for(int64_t val : prevzoneN){
        if(!zoneN.contains(val)){
            glm::vec2 curr = toCoords(val);
            for(int x = curr.x; x < curr.x + 64; x+=16){
                for(int z = curr.y; z < curr.y + 64; z+=16){
                    auto& c = getChunkAt(x, z);
                    c->destroyVBOdata();
                }
            }
        }
    }

    for(int64_t val : zoneN){
        glm::vec2 curr = toCoords(val);
        int xFloor = static_cast<int>(glm::floor(curr.x / 64.f)) * 64;
        int zFloor = static_cast<int>(glm::floor(curr.y / 64.f)) * 64;
        //if it does exist then we create a vbo worker
        if((m_generatedTerrain.find(toKey(xFloor, zFloor)) != m_generatedTerrain.end())){
            if(!prevzoneN.contains(val)){
                for(int x = curr.x; x < curr.x + 64; x+=16){
                    for(int z = curr.y; z < curr.y + 64; z+=16){
                        uPtr<Chunk>& c = getChunkAt(x, z);
                        //computes interleaved data for this chunk
                        createVBOWorker(c.get());
                        //                          chunkVBOs.push_back(c.get());
                    }
                }
            }
        }else{
            //if it doesnt exist then we create a block type worker
            //will fill the zone based on create chunk
            createBlockTypeWorker(val);
        }
    }

    checkDataStates();
}

void Terrain::createVBOWorker(Chunk* currChunk){
    VBOWorker* worker = new VBOWorker(&mutexChunk, currChunk, &chunkVBOs);
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::createBlockTypeWorker(int64_t val){
    glm::vec2 curr = toCoords(val);
    m_generatedTerrain.insert(toKey(curr.x, curr.y));
    //    glm::vec2 zoneOrigin = glm::vec2(floor(curr.x / 16.f) * 16, floor(curr.y / 16.f) * 16);
    std::vector<Chunk*> workerChunks;
    for(int x = curr.x; x < curr.x + 64; x+=16){
        for(int z = curr.y; z < curr.y + 64; z+=16){
            Chunk* chunk = instantiateChunkAt(x, z);
            chunk->m_countOpq = 0;
            chunk->m_countTrans = 0;
            workerChunks.push_back(chunk);
        }
    }
    //        for(auto c : workerChunks){
    //            chunkBlockType.push_back(c);
    //        }
    BlockTypeWorker* worker = new BlockTypeWorker(&mutexBlock, workerChunks, &chunkBlockType, curr);
    //    worker->terrain = this;
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::checkDataStates(){
    mutexBlock.lock();
    for(Chunk* c : chunkBlockType){
        //check neighbors
        int x = c->x;
        int z = c->z;


        if(hasChunkAt(x, z + 16)) {
            auto &chunkNorth = m_chunks[toKey(x, z + 16)];
            if(chunkNorth->generated){
                createVBOWorker(chunkNorth.get());
            }
        }

        if(hasChunkAt(x, z - 16)) {
            auto &chunkSouth = m_chunks[toKey(x, z - 16)];
            if(chunkSouth->generated){
                createVBOWorker(chunkSouth.get());
            }
        }

        if(hasChunkAt(x + 16, z) && c->generated) {
            auto &chunkEast = m_chunks[toKey(x + 16, z)];
            if(chunkEast->generated){
                createVBOWorker(chunkEast.get());
            }
        }

        if(hasChunkAt(x - 16, z) && c->generated) {
            auto &chunkWest = m_chunks[toKey(x - 16, z)];
            if(chunkWest->generated){
                createVBOWorker(chunkWest.get());
            }
        }
        createVBOWorker(c);
    }

    //        for(auto* c : chunkBlockType){
    //            chunkVBOs.push_back(c);
    //        }
    chunkBlockType.clear();
    mutexBlock.unlock();
//    CreateRiver();
    mutexChunk.lock();
    for(ChunkData c : chunkVBOs){
        //update VBO data with  and opaque
        c.chunk->createVBO(c.opaq, c.opaqIdx, c.trans, c.transIdx);
    }
    chunkVBOs.clear();
    mutexChunk.unlock();
}



void Terrain::CreateRiver(){
    River riv = River("F[FFF]-F", 3, glm::vec2(92, -25), glm::vec2(-1, -1), 9);
    QString grammar = riv.currAxiom;
    for(int i = 0; i < grammar.length(); i++){
        //get beginning segment
        glm::vec2 startPos = riv.turtle.pos;
        //do the correct function for the turtle
        (riv.*(riv.charFunction[grammar[i]]))();
        //get the end of the segment
        glm::vec2 endPos = riv.turtle.pos;

        //if it is f then we know the turtle moves
        if (grammar[i] == 'F'){
            int minZ = endPos[1];
            int maxZ = startPos[1];
            int radius = 7;

            if (startPos[1] <= endPos[1]) {
                minZ = startPos[1];
            }else if (endPos[1] >= startPos[1]) {
                maxZ = endPos[1];
            }

            for(int z = minZ; z <= maxZ; z++){
                float intercept = startPos[0];
                //if the start of our segment doesnt equal the end of our segment x axis then we need to get the intercept
                if (startPos[0] != endPos[0]) {
                    intercept = (z - startPos[1]) / ((startPos[1] - endPos[1]) / (startPos[0] - endPos[0])) + startPos[0];
                }
                intercept = glm::floor(intercept);
                for (int x = -radius; x <= radius; x++) {
                    int water = 140;
                    // add water
                    //set all above to empty
                    for(int y = water; y < 256; y++){
                        setBlockAt(intercept+x, y, z, EMPTY);
                    }
                    //hemisphere
                    for (int y = -radius; y < 0; y++) {
                        //based off lecture to carve
                        float dist = glm::length(glm::vec3(intercept + x, water + y, z) - glm::vec3(intercept, water, z));
                        if (dist < radius) {
                            if(((water + y) < water)){
                                setBlockAt(intercept + x, y + water, z, WATER);
                            }
                        }
                    }
                }
            }
        }
    }
}

