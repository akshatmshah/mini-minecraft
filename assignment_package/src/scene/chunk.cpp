#include "chunk.h"
#include <iostream>


Chunk::Chunk(OpenGLContext* context, int x, int z, const std::vector<uPtr<Model>> &models) : Drawable(context), m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}, generated(false), x(x), z(z), transIdx{}, trans{}, opaqIdx{}, opaq{}, npcs{}, models(models) /*chunkData(this)*/
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    //    if(x > 15 || y > 255 || z > 15 || x < 0 || y < 0 || z < 0){
    //        return BlockType::EMPTY;
    //    }

    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    //    if(x > 15 || y > 255 || z > 15 || x < 0 || y < 0 || z < 0){
    //        return BlockType::EMPTY;
    //    }
//    std::cout << "-----------" << std::endl;
//    std::cout << x << " " << y << " " << z << std::endl;
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

const static std::array<BlockNeighbor, 6> faceDirection = {

    BlockNeighbor(XPOS, glm::vec3(1, 0, 0), BlockVertex(glm::vec4(1, 0, 1, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 0, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 1, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(XNEG, glm::vec3(-1, 0, 0), BlockVertex(glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 0, 1, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 1, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(YPOS, glm::vec3(0, 1, 0), BlockVertex(glm::vec4(0, 1, 1, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 1, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(YNEG, glm::vec3(0, -1, 0), BlockVertex(glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 0, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 1, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 0, 1, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(ZPOS, glm::vec3(0, 0, 1), BlockVertex(glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 1, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 1, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 1, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(ZNEG, glm::vec3(0, 0, -1), BlockVertex(glm::vec4(1, 0, 0, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 0, 0, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0, 1), glm::vec4(0, 0.0625, 0, 0)))
};

//face direction for cross direcative blocks
const static std::array<BlockNeighbor, 6> faceDirectionDec = {
    BlockNeighbor(XPOS, glm::vec3(1, 0, 0), BlockVertex(glm::vec4(0.5, 0, 1, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(0.5, 0, 0, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(0.5, 1, 0, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0.5, 1, 1, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(XNEG, glm::vec3(-1, 0, 0), BlockVertex(glm::vec4(0.5, 0, 0, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(0.5, 0, 1, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(0.5, 1, 1, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0.5, 1, 0, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(YPOS, glm::vec3(0, 1, 0), BlockVertex(glm::vec4(0, 1, 1, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 1, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(YNEG, glm::vec3(0, -1, 0), BlockVertex(glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 0, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 1, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 0, 1, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(ZPOS, glm::vec3(0, 0, 1), BlockVertex(glm::vec4(0, 0, 0.5, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 0, 0.5, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0.5, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0.5, 1), glm::vec4(0, 0.0625, 0, 0))),

    BlockNeighbor(ZNEG, glm::vec3(0, 0, -1), BlockVertex(glm::vec4(1, 0, 0.5, 1), glm::vec4(0, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 0, 0.5, 1), glm::vec4(0.0625, 0, 0, 0)),
    BlockVertex(glm::vec4(0, 1, 0.5, 1), glm::vec4(0.0625, 0.0625, 0, 0)),
    BlockVertex(glm::vec4(1, 1, 0.5, 1), glm::vec4(0, 0.0625, 0, 0)))
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

bool Chunk::transparent(BlockType t) {
    return t == WATER || /*t == ICE ||*/ t == LAVA || t == WINDOW || t == BIRCHLEAF || t == PALMLEAF || t == ACACIALEAF || cross(t);
}

bool Chunk::cross(BlockType t) {
    return t == GRASSDEC || t == SHRUB || t == BROWNMUSH || t == REDMUSH || t == DAISY || t == ROSE;
}

void Chunk::createVBOdata(){
    //opaque
    std::vector<glm::vec4> vbo_opq;
    std::vector<GLuint> idx_opq;

    //transparent
    std::vector<glm::vec4> vbo_trans;
    std::vector<GLuint> idx_trans;

    int count_opq = 0;
    int count_trans = 0;

    for(int x = 0; x < 16; x++){
        for(int y = 0; y < 256; y++){
            for(int z = 0; z < 16; z++){
                BlockType currBlockType = getBlockAt(x, y, z);

                if(currBlockType != BlockType::EMPTY){

                    glm::vec4 currBlock = glm::vec4(x, y, z, 0);

                    //neighbors
                    BlockType posXBlock;
                    BlockType negXBlock;
                    BlockType posYBlock;
                    BlockType negYBlock;
                    BlockType posZBlock;
                    BlockType negZBlock;

                    if (x == 0) {
                        if (m_neighbors[XNEG] != nullptr) {
                            negXBlock = m_neighbors[XNEG]->getBlockAt(15, y ,z);
                        } else {
                            negXBlock = EMPTY;
                        }
                        posXBlock = getBlockAt(x + 1, y, z);
                    } else if (x == 15) {
                        if (m_neighbors[XPOS] != nullptr) {
                            posXBlock = m_neighbors[XPOS]->getBlockAt(0, y, z);
                        } else {
                            posXBlock = EMPTY;
                        }
                        negXBlock = getBlockAt(x - 1, y, z);
                    } else {
                        negXBlock = getBlockAt(x - 1, y, z);
                        posXBlock = getBlockAt(x + 1, y, z);
                    }

                    if (y == 0) {
                        negYBlock = EMPTY;
                        posYBlock = getBlockAt(x, y + 1, z);
                    } else if (y == 255) {
                        negYBlock = getBlockAt(x, y - 1, z);
                        posYBlock = EMPTY;
                    } else {
                        negYBlock = getBlockAt(x, y - 1, z);
                        posYBlock = getBlockAt(x, y + 1, z);
                    }

                    if (z == 0) {
                        if (m_neighbors[ZNEG] != nullptr) {
                            negZBlock = m_neighbors[ZNEG]->getBlockAt(x, y, 15);
                        } else {
                            negZBlock = EMPTY;
                        }
                        posZBlock = getBlockAt(x, y, z + 1);
                    } else if (z == 15) {
                        if (m_neighbors[ZPOS] != nullptr) {
                            posZBlock = m_neighbors[ZPOS]->getBlockAt(x, y, 0);
                        } else {
                            posZBlock = EMPTY;
                        }
                        negZBlock = getBlockAt(x, y, z - 1);
                    } else {
                        posZBlock = getBlockAt(x, y, z + 1);
                        negZBlock = getBlockAt(x, y, z - 1);
                    }
                    if(!cross(currBlockType)){
                        if(posXBlock == BlockType::EMPTY || (transparent(posXBlock) && posXBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, XPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, XPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negXBlock == BlockType::EMPTY || (transparent(negXBlock) && negXBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, XNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, XNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(posYBlock == BlockType::EMPTY || (transparent(posYBlock) && posYBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, YPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, YPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negYBlock == BlockType::EMPTY || (transparent(negYBlock) && negYBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, YNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, YNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(posZBlock == BlockType::EMPTY || (transparent(posZBlock) && posZBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, ZPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, ZPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negZBlock == BlockType::EMPTY || (transparent(negZBlock) && negZBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, ZNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, ZNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }
                    }else{
                        if(posXBlock == BlockType::EMPTY || (transparent(posXBlock) && posXBlock != currBlockType || transparent(currBlockType))) {
                                updateVBOdata(vbo_trans, XPOS, idx_trans, count_trans, currBlock, currBlockType);
                        }

                        if(posZBlock == BlockType::EMPTY || (transparent(posZBlock) && posZBlock != currBlockType || transparent(currBlockType))) {
                                updateVBOdata(vbo_trans, ZPOS, idx_trans, count_trans, currBlock, currBlockType);
                     }
                }
            }
        }
       }
    }
    opaq = vbo_opq;
    opaqIdx = idx_opq;
    trans = vbo_trans;
    transIdx = idx_trans;

    m_countOpq = idx_opq.size();
    m_countTrans = idx_trans.size();
}

void Chunk::createVBOdata(ChunkData* chunkData){
    //opaque
    std::vector<glm::vec4> vbo_opq;
    std::vector<GLuint> idx_opq;

    //transparent
    std::vector<glm::vec4> vbo_trans;
    std::vector<GLuint> idx_trans;

    int count_opq = 0;
    int count_trans = 0;

    for(int x = 0; x < 16; x++){
        for(int y = 0; y < 256; y++){
            for(int z = 0; z < 16; z++){
                BlockType currBlockType = getBlockAt(x, y, z);

                if(currBlockType != BlockType::EMPTY){

                    glm::vec4 currBlock = glm::vec4(x, y, z, 0);

                    //neighbors
                    BlockType posXBlock;
                    BlockType negXBlock;
                    BlockType posYBlock;
                    BlockType negYBlock;
                    BlockType posZBlock;
                    BlockType negZBlock;

                    if (x == 0) {
                        if (m_neighbors[XNEG] != nullptr) {
                            negXBlock = m_neighbors[XNEG]->getBlockAt(15, y ,z);
                        } else {
                            negXBlock = EMPTY;
                        }
                        posXBlock = getBlockAt(x + 1, y, z);
                    } else if (x == 15) {
                        if (m_neighbors[XPOS] != nullptr) {
                            posXBlock = m_neighbors[XPOS]->getBlockAt(0, y, z);
                        } else {
                            posXBlock = EMPTY;
                        }
                        negXBlock = getBlockAt(x - 1, y, z);
                    } else {
                        negXBlock = getBlockAt(x - 1, y, z);
                        posXBlock = getBlockAt(x + 1, y, z);
                    }

                    if (y == 0) {
                        negYBlock = EMPTY;
                        posYBlock = getBlockAt(x, y + 1, z);
                    } else if (y == 255) {
                        negYBlock = getBlockAt(x, y - 1, z);
                        posYBlock = EMPTY;
                    } else {
                        negYBlock = getBlockAt(x, y - 1, z);
                        posYBlock = getBlockAt(x, y + 1, z);
                    }

                    if (z == 0) {
                        if (m_neighbors[ZNEG] != nullptr) {
                            negZBlock = m_neighbors[ZNEG]->getBlockAt(x, y, 15);
                        } else {
                            negZBlock = EMPTY;
                        }
                        posZBlock = getBlockAt(x, y, z + 1);
                    } else if (z == 15) {
                        if (m_neighbors[ZPOS] != nullptr) {
                            posZBlock = m_neighbors[ZPOS]->getBlockAt(x, y, 0);
                        } else {
                            posZBlock = EMPTY;
                        }
                        negZBlock = getBlockAt(x, y, z - 1);
                    } else {
                        posZBlock = getBlockAt(x, y, z + 1);
                        negZBlock = getBlockAt(x, y, z - 1);
                    }

                    if(!cross(currBlockType)){
                        if(posXBlock == BlockType::EMPTY || (transparent(posXBlock) && posXBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, XPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, XPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negXBlock == BlockType::EMPTY || (transparent(negXBlock) && negXBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, XNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, XNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(posYBlock == BlockType::EMPTY || (transparent(posYBlock) && posYBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, YPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, YPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negYBlock == BlockType::EMPTY || (transparent(negYBlock) && negYBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, YNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, YNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(posZBlock == BlockType::EMPTY || (transparent(posZBlock) && posZBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, ZPOS, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, ZPOS, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }

                        if(negZBlock == BlockType::EMPTY || (transparent(negZBlock) && negZBlock != currBlockType)) {
                            if (transparent(currBlockType)) {
                                updateVBOdata(vbo_trans, ZNEG, idx_trans, count_trans, currBlock, currBlockType);
                            } else {
                                updateVBOdata(vbo_opq, ZNEG, idx_opq, count_opq, currBlock, currBlockType);
                            }
                        }
                    }else{
                        if(posXBlock == BlockType::EMPTY || (transparent(posXBlock) && posXBlock != currBlockType || transparent(currBlockType))) {
                                updateVBOdata(vbo_trans, XPOS, idx_trans, count_trans, currBlock, currBlockType);
                        }

                        if(posZBlock == BlockType::EMPTY || (transparent(posZBlock) && posZBlock != currBlockType || transparent(currBlockType))) {
                                updateVBOdata(vbo_trans, ZPOS, idx_trans, count_trans, currBlock, currBlockType);
                     }
                }
                }
            }
        }
    }
    chunkData->opaq = vbo_opq;
    chunkData->opaqIdx = idx_opq;
    chunkData->trans = vbo_trans;
    chunkData->transIdx = idx_trans;

    opaq = vbo_opq;
    opaqIdx = idx_opq;
    trans = vbo_trans;
    transIdx = idx_trans;


    m_countOpq = idx_opq.size();
    m_countTrans = idx_trans.size();
}


//update VBO
//for loop is in this, called each time update is called instead of in create
void Chunk::updateVBOdata(std::vector<glm::vec4> &vbo, Direction d, std::vector<GLuint> &idx, int &count, glm::vec4 currPos, BlockType blockType) {
    idx.push_back(count);
    idx.push_back(count + 1);
    idx.push_back(count + 2);
    idx.push_back(count);
    idx.push_back(count + 2);
    idx.push_back(count + 3);
    count = count + 4;

    //    Face currFace = faceDirection.at(d);
    //    glm::vec4 normal = currFace.normal;
    //    for (int i = 0; i < 4; i++){
    //        vbo.push_back(currFace.BlockVertexPos.at(i) + currPos);
    //        vbo.push_back(normal);
    //        vbo.push_back(color);
    //    }
    if(cross(blockType)){
       auto neighbor = faceDirectionDec.at(d);
       glm::vec4 neighbor_uv = blockFaceUVs.at(blockType).at(d);

       for (int i = 0; i < 4; i++) {
           vbo.push_back(neighbor.vertices[i].pos + currPos);
           vbo.push_back(glm::vec4(neighbor.offset, 1));
           vbo.push_back(neighbor_uv + neighbor.vertices[i].uv);
       }
    }else{
       auto neighbor = faceDirection.at(d);
       glm::vec4 neighbor_uv = blockFaceUVs.at(blockType).at(d);

       for (int i = 0; i < 4; i++) {
           vbo.push_back(neighbor.vertices[i].pos + currPos);
           vbo.push_back(glm::vec4(neighbor.offset, 1));

           if (blockType == LAVA || blockType == WATER) {
               neighbor_uv.z = 1;
           }
           vbo.push_back(neighbor_uv + neighbor.vertices[i].uv);
       }
    }
}

void Chunk::createVBO(std::vector<glm::vec4> &vbo_opq, std::vector<GLuint> &idx_opq, std::vector<glm::vec4> &vbo_trans, std::vector<GLuint> &idx_trans){
    generateOpq();
    bindOpq();
    mp_context->glBufferData(GL_ARRAY_BUFFER, vbo_opq.size() * sizeof(glm::vec4), vbo_opq.data(), GL_STATIC_DRAW);

    generateIdxOpq();
    bindIdxOpq();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_opq.size() * sizeof(GLuint), idx_opq.data(), GL_STATIC_DRAW);

    generateTrans();
    bindTrans();
    mp_context->glBufferData(GL_ARRAY_BUFFER, vbo_trans.size() * sizeof(glm::vec4), vbo_trans.data(), GL_STATIC_DRAW);

    generateIdxTrans();
    bindIdxTrans();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_trans.size() * sizeof(GLuint), idx_trans.data(), GL_STATIC_DRAW);
}

int Chunk::grasslandValue(glm::vec2 uv){
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = glm::smoothstep(0.f, 1.f, worley);
    worley = std::clamp(worley * 0.4f + ((Noise::perlinNoise2D(uv / 256.f) + 1) / 2.f) * 0.6f, 0.f, 1.f);
    return worley * 30.f + 130.f;
}

// Given a world-space coordinate, produces a y coordinate that represents the
// height of the mountain biome at that point
int Chunk::mountainValue(glm::vec2 uv){
    float perlin = (Noise::perlinNoise2D(uv / 32.f) + 1) / 2.f;
    return glm::pow(perlin, 3.f) * 115.f + 140.f;

}

int Chunk::driftingIslesValue(glm::vec2 uv){
    float worley = Noise::WorleyNoise(uv / 72.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = std::clamp(worley * 0.4f + ((Noise::perlinNoise2D(uv / 256.f) + 1) / 2.f) * 0.6f, 0.f, 1.f);
    return worley * 20.f + 130.f;
}

int Chunk::desertValue(glm::vec2 uv){
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = std::clamp(worley * 0.33f + ((Noise::fractalBrownianMotion(uv / 256.f) + 1) / 2.f) * 0.67f, 0.f, 1.f);
    return glm::pow(worley, 2.f) * 30.f + 135.f;
}

int Chunk::lavalandValue(glm::vec2 uv) {
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = glm::smoothstep(0.f, 1.f, worley);
    worley = std::clamp(worley * 0.4f + ((Noise::perlinNoise2D(uv / 256.f) + 1) / 2.f) * 0.6f, 0.f, 1.f);
    //    return worley * 20.f + 70.f;
    return worley * 20.f + 100.f;
}

int Chunk::mushroomlandValue(glm::vec2 uv) {
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = glm::smoothstep(0.f, 1.f, worley);
    worley = std::clamp(worley * 0.4f + ((Noise::perlinNoise2D(uv / 256.f) + 1) / 2.f) * 0.6f, 0.f, 1.f);
    return worley * 30.f + 126.f;
}

int Chunk::tundraValue(glm::vec2 uv) {
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = glm::smoothstep(0.f, 1.f, worley);
    worley = std::clamp(worley * 0.2f + ((Noise::perlinNoise2D(uv / 64.f) + 1) / 2.f) * 0.8f, 0.f, 1.f);
    return worley * 40.f + 124.f;
}

int Chunk::islandValue(glm::vec2 uv) {
    float perlin = (Noise::perlinNoise2D(uv / 32.f) + 1) / 2.f;
    return glm::pow(perlin, 2.f) * 60.f + 50.f;
}

int Chunk::mesaValue(glm::vec2 uv) {
    float worley = Noise::WorleyNoise(uv / 64.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = glm::smoothstep(0.f, 1.f, worley);
    int plateauHeight = worley * 10.f + 170.f;

    worley = Noise::WorleyNoise(uv / 32.f);
    worley = std::max(0.f, worley - 0.1f);
    worley = std::clamp(worley * 0.33f + ((Noise::fractalBrownianMotion(uv / 128.f) + 1) / 2.f) * 0.67f, 0.f, 1.f);
    int desertHeight = glm::pow(worley, 2.f) * 40.f + 135.f;

    float mesaNoise = (Noise::perlinNoise2D(uv / 32.f) + 1) / 2.f;
    mesaNoise = glm::smoothstep(.45f, .55f, mesaNoise);
    return glm::mix(plateauHeight, desertHeight, mesaNoise);
}

void Chunk::CreateChunk(int x, int z) {
    bool createCaves = true;
    std::vector<glm::ivec2> spawnPositions;
    //1/16 chance to spawn npc
    if (rand() % 16 == 0) {
        do {
            if (spawnPositions.size() > 0) {
                //picks new random block in 5x5 radius around previous block that was chosen to spawn NPC within in the chunk
                spawnPositions.push_back(glm::vec2(std::clamp(spawnPositions[spawnPositions.size() - 1][0] + (rand() % 5 - 2), 0, 15), std::clamp(spawnPositions[spawnPositions.size() - 1][1] + (rand() % 5 - 2), 0, 15)));
            } else {
                spawnPositions.push_back(glm::vec2(rand() % 16, rand() % 16));
            }
        } while(rand() % 2 == 0 && spawnPositions.size() <= 4);

    }
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 16; j++) {
            int worldX = x + i;
            int worldZ = z + j;
            float moisture = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 512.f) + 1) / 2.f;
            moisture = glm::smoothstep(.45f, .55f, moisture);
            float temperature = (Noise::perlinNoise2D(glm::vec2(-worldX, worldZ) / 512.f) + 1) / 2.f;
            temperature = glm::smoothstep(.45f, .55f, temperature);
            float pressure = (Noise::perlinNoise2D(glm::vec2(worldX, -worldZ) / 512.f) + 1) / 2.f;
            pressure = glm::smoothstep(.45f, .55f, pressure);
            int mix1 = glm::mix(mountainValue(glm::vec2(worldX, worldZ)), grasslandValue(glm::vec2(worldX, worldZ)), moisture);
            int mix2 = glm::mix(desertValue(glm::vec2(worldX, worldZ)), driftingIslesValue(glm::vec2(worldX, worldZ)), moisture);
            int mix3 = glm::mix(mushroomlandValue(glm::vec2(worldX, worldZ)), tundraValue(glm::vec2(worldX, worldZ)), moisture);
            int mix4 = glm::mix(mesaValue(glm::vec2(worldX, worldZ)), islandValue(glm::vec2(worldX, worldZ)), moisture);
            int mixA = glm::mix(mix1, mix2, temperature);
            int mixB = glm::mix(mix3, mix4, temperature);
            int yPeak = glm::mix(mixA, mixB, pressure);
            setBlockAt(i, 0, j, BEDROCK);
            if (moisture >= 0.5) {
                if (temperature >= 0.5) {
                    if (pressure >= 0.5) {
                        //island
                        float islandNoise = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 32.f) + 1) / 2.f;
                        float treeNoise = Noise::random1(glm::vec2(worldX, worldZ));
                        int depth, delta, islandPeak;
                        if (islandNoise >= 0.6) {
                            float worley = Noise::WorleyNoise(glm::vec2(worldX, worldZ) / 72.f);
                            worley = glm::smoothstep(0.f, 1.f, worley);
                            worley = std::clamp(worley * 0.2f + islandNoise * 0.8f, 0.f, 1.f);

                            float perlin = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 8.f) + 1) / 2.f;
                            depth = glm::pow(perlin, 3.f) * 20.f + 5.f;
                            delta = 53;
                            islandPeak = (worley * 40.f + 65.f) + delta;
                            for (int y = islandPeak - depth; y <= islandPeak; y++) {
                                if (y <= 137 && y != islandPeak) {
                                    setBlockAt(i, y, j, STONE);
                                } else if (y <= 140) {
                                    setBlockAt(i, y, j, SAND);
                                } else if (y <= islandPeak - 3) {
                                    setBlockAt(i, y, j, STONE);
                                } else if (y == islandPeak){
                                    setBlockAt(i, y, j, GRASS);
                                } else {
                                    setBlockAt(i, y, j, DIRT);
                                }
                            }
                            for (int y = islandPeak + 1; y <= 138; y++) {
                                setBlockAt(i, y, j, WATER);
                            }
                            for (int y = yPeak + 1; y <= std::min(138, islandPeak - depth); y++) {
                                setBlockAt(i, y, j, WATER);
                            }
                            if (treeNoise >= 0.6 && treeNoise <= 0.675 && islandPeak >= 138) {
                                if (i >= 6 && i <= 9 && j >= 6 && j <= 9) {
                                    drawPalm(i, j, islandPeak + 1);
                                }
                            }
                            if (islandPeak >= 138) {
                                for (int x = 0; x < (int) spawnPositions.size(); x++){
                                    if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                        uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, islandPeak + 3, worldZ), this, models[2].get(), 2);
                                        npc->createVBOdata();
                                        this->npcs.push_back(std::move(npc));
                                    }
                                }
                            } else {
                                for (int x = 0; x < (int) spawnPositions.size(); x++){
                                    if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                        uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[5].get(), 5);
                                        npc->createVBOdata();
                                        this->npcs.push_back(std::move(npc));
                                    }
                                }
                            }
                        } else {
                            for (int y = yPeak + 1; y <= 138; y++) {
                                setBlockAt(i, y, j, WATER);
                            }
                        }
                        for (int y = 1; y <= yPeak; y++) {
                            if (y == yPeak) {
                                setBlockAt(i, y, j, SAND);
                            } else if (y <= 70) {
                                if (createCaves) {
                                    float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                    if (caveNoise < -0.15) {
                                        if (y < 25) {
                                            setBlockAt(i, y, j, LAVA);
                                        } else {
                                            setBlockAt(i, y, j, EMPTY);
                                        }
                                    } else {
                                        setBlockAt(i, y, j, STONE);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, SAND);
                            }
                        }

                } else {
                    //driftland
                    float craterNoise = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 32.f) + 1) / 2.f;
                    float treeNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    int depth, delta, islandPeak;
                    if (craterNoise >= 0.58) {
                        glm::vec2 cellCenter;
                        float worley = Noise::WorleyNoise(glm::vec2(worldX, worldZ) / 72.f, &cellCenter);
                        worley = std::max(0.f, worley - 0.1f);
                        worley = glm::smoothstep(0.f, 1.f, worley);
                        worley = std::clamp(worley * 0.4f + ((Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 256.f) + 1) / 2.f) * 0.6f, 0.f, 1.f);

                        float perlin = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 8.f) + 1) / 2.f;
                        depth = glm::pow(perlin, 3.f) * 25.f + 5.f;
                        delta = 100 * (0.7 * Noise::random1(cellCenter) + 0.3);
                        islandPeak = (worley * 20.f + 130.f) + delta;
                        for (int y = islandPeak - depth; y <= islandPeak; y++) {
                            if (y == islandPeak) {
                                setBlockAt(i, y, j, GRASS);
                            } else if (y <= 126 + delta || y <= islandPeak - 5) {
                                setBlockAt(i, y, j, STONE);
                            } else {
                                setBlockAt(i, y, j, DIRT);
                            }
                        }
                        if (islandPeak >= 138 && islandPeak <= 247) {
                            if (treeNoise >= 0.6 && treeNoise <= 0.65) {
                                if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                                   drawBirch(i, j, islandPeak + 1);
                                }
                            } else if (treeNoise >= 0.3 && treeNoise <= 0.35) {
                                if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                                   drawOak(i, j, islandPeak + 1);
                                }
                            } else if (treeNoise >= 0.1 && treeNoise <= 0.11) {
                                setBlockAt(i, islandPeak + 1, j, DAISY);
                            } else if (treeNoise >= 0.2 && treeNoise <= 0.21) {
                                setBlockAt(i, islandPeak + 1, j, ROSE);
                            }
                        }
                        yPeak -= depth;
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 126 || y <= yPeak - 5) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }
                        } else if (y == yPeak && craterNoise < 0.58) {
                            setBlockAt(i, y, j, GRASS);
                        } else {
                            setBlockAt(i, y, j, DIRT);
                        }
                    }
                    if (craterNoise < 0.58 && yPeak >= 138) {
                        if (treeNoise >= 0.6 && treeNoise <= 0.65) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                               drawBirch(i, j, yPeak + 1);
                            }
                        } else if (treeNoise >= 0.3 && treeNoise <= 0.35) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                               drawOak(i, j, yPeak + 1);
                            }
                        } else if (treeNoise >= 0.1 && treeNoise <= 0.11) {
                            setBlockAt(i, yPeak + 1, j, DAISY);
                        } else if (treeNoise >= 0.2 && treeNoise <= 0.21) {
                            setBlockAt(i, yPeak + 1, j, ROSE);
                        }
                    }
                    if (yPeak >= 138) {
                        for (int x = 0; x < (int) spawnPositions.size(); x++){
                            if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[4].get(), 4);
                                npc->createVBOdata();
                                this->npcs.push_back(std::move(npc));
                            }
                        }
                    }
                }
            } else {
                if (pressure >= 0.5) {
                    //tundra
                    float treeNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 115) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }
                        } else if (y == yPeak && yPeak >= 138) {
                            setBlockAt(i, y, j, SNOWDIRT);
                        } else {
                            setBlockAt(i, y, j, DIRT);
                        }
                    }
                    float iceNoise = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 32.f) + 1) / 2.f;
                    if ((iceNoise >= 0.4 && iceNoise <= 0.6) || 138 - yPeak <= 3) {
                        for (int y = yPeak + 1; y <= 138; y++) {
                            setBlockAt(i, y, j, ICE);
                        }
                    } else {
                        for (int y = yPeak + 1; y <= 138; y++) {
                            setBlockAt(i, y, j, WATER);
                        }
                    }
                    if (yPeak >= 138) {
                        if (treeNoise >= 0.7 && treeNoise <= 0.74) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                                drawLorgeSnowy(i, j, yPeak + 1);
                            }
                        } else if (treeNoise <= 0.0005) {
                            if (i >= 1 && i <= 14 && j >= 1 && j <= 14) {
                                drawSmolSnowy(i, j, yPeak + 1);
                            }
                        } else if (treeNoise >= 0.3 && treeNoise <= 0.32) {
                            if (i >= 1 && i <= 14 && j >= 1 && j <= 14) {
                                drawTallSnowy(i, j, yPeak + 1);
                            }
                        }
                    }
                    if (yPeak >= 138) {
                        for (int x = 0; x < (int) spawnPositions.size(); x++){
                            if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[0].get(), 0);
                                npc->createVBOdata();
                                this->npcs.push_back(std::move(npc));
                            }
                        }
                    }
                } else {
                    //grassland
                    float treeNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 100) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }
                        } else if (y <= 138) {
                            setBlockAt(i, y, j, SAND);
                        } else if (y == yPeak) {
                            setBlockAt(i, y, j, GRASS);
                        } else {
                            setBlockAt(i, y, j, DIRT);
                        }
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    if (yPeak >= 139) {
                        if (treeNoise >= 0.6 && treeNoise <= 0.6025) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                               drawOak(i, j, yPeak + 1);
                            }
                        } else {
                            float grassNoise = Noise::WorleyNoise(glm::vec2(worldX, worldZ));
                            if (grassNoise >= 0.6) {
                                setBlockAt(i, yPeak + 1, j, GRASSDEC);
                            } else if (grassNoise >= 0.35 && grassNoise <= 0.355) {
                                setBlockAt(i, yPeak + 1, j, ROSE);
                            } else if (grassNoise >= 0.345 && grassNoise <= 0.35)  {
                                setBlockAt(i, yPeak + 1, j, DAISY);
                            }
                        }
                    }
                    if (yPeak >= 138) {
                        int animalRand = rand() % 5;
                        for (int x = 0; x < (int) spawnPositions.size(); x++){
                            if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[animalRand].get(), animalRand);
                                npc->createVBOdata();
                                this->npcs.push_back(std::move(npc));
                            }
                        }
                    }
                }
            }
        } else {
            if (temperature >= 0.5) {
                if (pressure >= 0.5) {
                    //mesa
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 100) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }
                        } else if (y <= 150) {
                            setBlockAt(i, y, j, REDSAND); //red sand
                        } else if (y % 20 < 5) {
                            setBlockAt(i, y, j, BROWN);
                        } else if (y % 20 < 8) {
                            setBlockAt(i, y, j, RED);
                        } else if (y % 20 < 12) {
                            setBlockAt(i, y, j, ORANGE);
                        } else if (y % 20 < 17) {
                            setBlockAt(i, y, j, YELLOW);
                        } else if (y % 20 < 20) {
                            setBlockAt(i, y, j, WHITE);
                        }
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    if (yPeak >= 138) {
                        int animalRand = rand() % 2;
                        for (int x = 0; x < (int) spawnPositions.size(); x++){
                            if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[animalRand].get(), animalRand);
                                npc->createVBOdata();
                                this->npcs.push_back(std::move(npc));
                            }
                        }
                    }
                    float treeNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    if (yPeak >= 138) {
                        if (treeNoise >= 0.7 && treeNoise <= 0.705) {
                            if (i >= 6 && i <= 9 && j >= 6 && j <= 9) {
                                drawSmolAcacia(i, j, yPeak + 1);
                            }
                        } else if (treeNoise <= 0.0005) {
                            if (i >= 6 && i <= 9 && j >= 6 && j <= 9) {
                                drawLorgeAcacia(i, j, yPeak + 1);
                            }
                        } else if (treeNoise >= 0.2 && treeNoise <= 0.205) {
                            setBlockAt(i, yPeak + 1, j, SHRUB);
                        }
                    }
                } else {
                    //desert
                    float cactusNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 100) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }

                        } else {
                            setBlockAt(i, y, j, SAND);
                        }
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    if (cactusNoise >= 0.6 && cactusNoise <= 0.602 && yPeak >= 138) {
                        bool isXDir = (int)(Noise::random1(glm::vec2(-worldX, worldZ)) * 100.f) % 2 == 0;
                        if (isXDir) {
                            if (i >= 2 && i <= 13) {
                                drawCactus(i, j, yPeak + 1, cactusNoise * 4.f + 6, isXDir);
                            }
                        } else {
                            if (j >= 2 && j <= 13) {
                                drawCactus(i, j, yPeak + 1, cactusNoise * 4.f + 6, isXDir);
                            }
                        }
                    } else if (cactusNoise >= 0.2 && cactusNoise <= 0.205) {
                        setBlockAt(i, yPeak + 1, j, SHRUB);
                    }

                }
            } else {
                if (pressure >= 0.5) {
                    //mushroomland
                    float shroomNoise = Noise::random1(glm::vec2(worldX, worldZ));
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 120) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }

                        } else if (y == yPeak) {
                            setBlockAt(i, y, j, MYCELIUM);
                        } else {
                            setBlockAt(i, y, j, DIRT);
                        }
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    if (yPeak >= 138) {
                        if (shroomNoise >= 0.7 && shroomNoise <= 0.705) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                                drawRedShroom(i, j, yPeak + 1);
                            }
                        } else if (shroomNoise <= 0.0005) {
                            if (i >= 2 && i <= 13 && j >= 2 && j <= 13) {
                                drawBrownShroom(i, j, yPeak + 1);
                            }
                        } else {
                            float mushNoise = (Noise::perlinNoise2D(glm::vec2(worldX, worldZ) / 16.f) + 1) / 2.f;
                            if (mushNoise >= 0.65) {
                                setBlockAt(i, yPeak + 1, j, REDMUSH);
                            } else if (mushNoise >= 0.6) {
                                setBlockAt(i, yPeak + 1, j, BROWNMUSH);
                            }
                        }
                        for (int x = 0; x < (int) spawnPositions.size(); x++){
                            if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                                uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[6].get(), 6);
                                npc->createVBOdata();
                                this->npcs.push_back(std::move(npc));
                            }
                        }
                    }
                } else {
                    //mountain
                    for (int y = 1; y <= yPeak; y++) {
                        if (y <= 100) {
                            if (createCaves) {
                                float caveNoise = Noise::perlinNoise3D(glm::vec3(worldX, y, worldZ) / 24.f);
                                if (caveNoise < -0.15) {
                                    if (y < 25) {
                                        setBlockAt(i, y, j, LAVA);
                                    } else {
                                        setBlockAt(i, y, j, EMPTY);
                                    }
                                } else {
                                    setBlockAt(i, y, j, STONE);
                                }
                            } else {
                                setBlockAt(i, y, j, STONE);
                            }
                        } else if (y == yPeak && yPeak > (180 - (Noise::random2(glm::vec2(worldX, worldZ))[0] * 5.f))) {
                            setBlockAt(i, y, j, SNOW);
                        } else {
                            setBlockAt(i, y, j, STONE);
                        }
                    }
                    for (int y = yPeak + 1; y <= 138; y++) {
                        setBlockAt(i, y, j, WATER);
                    }
                    int animalRand = rand() % 3;
                    for (int x = 0; x < (int) spawnPositions.size(); x++){
                        if (i == spawnPositions[x][0] && j == spawnPositions[x][1]) {
                            uPtr<NPC> npc  = mkU<NPC>(mp_context, glm::vec3(worldX, yPeak + 3, worldZ), this, models[animalRand].get(), animalRand);
                            npc->createVBOdata();
                            this->npcs.push_back(std::move(npc));
                        }
                    }
                }
            }
        }
        }
    }
}

void Chunk::createRivers(){
    //first we need to get the grammar that was generated
    River riv = River("FF", 3, glm::vec2(0, 0), glm::vec2(0.5, 1.0), 10);
    QString grammar = riv.currAxiom;
    for(int i = 0; i < grammar.length(); i++){

    }
}
//void Chunk::drawStuff() {
//    for (int i = 0; i < 32; i++) {
//        int x = rand() % 80 + 1;
//        int z = rand() % 80 + 1;

//       int height = 0;
//         //find height
//        for (int h = 256; h >= 128; h--) {
//            if (getBlockAt(x, h, z) != EMPTY) {
//                height = h + 1;
//                break;
//            }
//        }
//        //snowy tree forest
//        if(i < 16) {
//            drawTree2(x, z, height, MYCELIUM);

//         } else {
//             drawTree3(x, z, height, SAND);
//         }
//    }

//    drawSpiky(0, 10, 0, 10, 30, 3, BLACK);
//    drawSpiky(30, 40, 30, 40, 40, 3, BLUE);
//    drawBoring(50, 60, 50, 60, 100, PURPLE);
//    drawBoring(15, 25, 35, 45, 80, GRAY);
//    drawSlanty(42, 52, 12, 22, 100, ORANGE);
//    drawSlanty(14, 24, 44, 54, 120, RED);



    // render trees
//    for (int x = -64; x <= 128; x += 16) {
//        for (int z = -64; z <= 128; z += 16) {
//            auto& chunk = getChunkAt(x, z);
//            chunk->destroyVBOdata();
//            chunk->createVBOdata();
//            chunk->createVBO(chunk->chunkData.opaq, chunk->chunkData.opaqIdx, chunk->chunkData.trans, chunk->chunkData.transIdx);
//        }
//    }
//}

void Chunk::drawRedShroom(int x, int z, int height) {
    // stem height 2
    for (int y = height; y < height + 2; y++) {
        setBlockAt(x, y, z, STEM);
    }
    //3x5 mushroom situation
    int bottom = height + 2;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = bottom; j < bottom + 3; j++) {
                setBlockAt(i, j, k, MUSHROOM);
            }
        }
    }
    //mushdoom sideburn situation
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 2; k <= z + 2; k+=4) {
            for (int j = bottom; j < bottom + 3; j++) {
                setBlockAt(i, j, k, MUSHROOM);
            }
        }
    }
    //mushroom top
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, bottom + 3, k, MUSHROOM);
        }
    }
}

void Chunk::drawBrownShroom(int x, int z, int height) {
    // stem height 2
    for (int y = height; y < height + 4; y++) {
        setBlockAt(x, y, z, STEM);
    }
    //3x5 mushroom situation
    int bottom = height + 4;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, bottom, k, BROWNSHROOM);
        }
    }
    //mushdoom sideburn situation
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 2; k <= z + 2; k+=4) {
            setBlockAt(i, bottom, k, BROWNSHROOM);
        }
    }
}

//minimum tallness is 6
void Chunk::drawCactus(int x, int z, int height, int tallness, bool xDir) {
    // stem height 2
    for (int y = height; y < height + tallness; y++) {
        setBlockAt(x, y, z, CACTUS);
    }
    //arms in x direction
    if (xDir) {
        setBlockAt(x-1, height + 2, z, CACTUS);
        setBlockAt(x+1, height + 3, z, CACTUS);
        for(int j = height + 2; j < height + tallness - 2; j++) {
            setBlockAt(x-2, j, z, CACTUS);
        }
        for(int j = height + 3; j < height + tallness - 1; j++) {
            setBlockAt(x+2, j, z, CACTUS);
        }
    } else {
        setBlockAt(x, height + 2, z-1, CACTUS);
        setBlockAt(x, height + 3, z+1, CACTUS);
        for(int j = height + 2; j < height + tallness - 2; j++) {
            setBlockAt(x, j, z-2, CACTUS);
        }
        for(int j = height + 3; j < height + tallness - 1; j++) {
            setBlockAt(x, j, z+2, CACTUS);
        }
    }

}

//trees lots of trees
void Chunk::drawBirch(int x, int z, int height) {
    // tree trunk height 2
    for (int y = height; y < height + 4; y++) {
        setBlockAt(x, y, z, BIRCH);
    }
    //bottom layer
    int bottom = height + 4;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int j = z - 2; j <= z + 2; j++) {
            if (getBlockAt(i, bottom, j) == EMPTY) {
                setBlockAt(i, bottom, j, BIRCHLEAF);
            }
        }
     }
    //middle
    int middle = height + 5;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = middle; j <= middle + 2; j+=2) {
                 setBlockAt(i, middle, k, BIRCHLEAF);
            }
        }
    }
    //top cross
    int top = height + 6;
    for (int i = x - 1; i <= x + 1; i++) {
       setBlockAt(i, top, z, BIRCHLEAF);
    }
    for (int j = z - 1; j <= z + 1; j++) {
       setBlockAt(x, top, j, BIRCHLEAF);
    }
    //post trunk
    for (int y = height + 4; y < height + 6; y++) {
        setBlockAt(x, y, z, BIRCH);
    }

}
//trees lots of trees
void Chunk::drawOak(int x, int z, int height) {
    // tree trunk height 2
    for (int y = height; y < height + 4; y++) {
        setBlockAt(x, y, z, WOOD);
    }
    //bottom layer
    int bottom = height + 4;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int j = z - 2; j <= z + 2; j++) {
            if (getBlockAt(i, bottom, j) == EMPTY) {
                setBlockAt(i, bottom, j, LEAF);
            }
        }
     }
    //middle
    int middle = height + 5;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = middle; j <= middle + 2; j+=2) {
                 setBlockAt(i, middle, k, LEAF);
            }
        }
    }
    //top cross
    int top = height + 6;
    for (int i = x - 1; i <= x + 1; i++) {
       setBlockAt(i, top, z, LEAF);
    }
    for (int j = z - 1; j <= z + 1; j++) {
       setBlockAt(x, top, j, LEAF);
    }
    //post trunk
    for (int y = height + 4; y < height + 6; y++) {
        setBlockAt(x, y, z, WOOD);
    }

}

void Chunk::drawLorgeSnowy(int x, int z, int height) {
    // stem height 7
    for (int y = height; y < height + 9; y++) {
        if (y == height + 8) {
             setBlockAt(x, y, z, SNOWLEAF);
        } else {
             setBlockAt(x, y, z, DARKWOOD);
        }
    }
    //layer 1 and 3
    int bottom = height + 2;
    for (int i = x - 2; i <= x + 2; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            for (int j = bottom; j <= bottom + 2; j+=2) {
                setBlockAt(i, j, k, SNOWLEAF);
            }
        }
    }
    //sideburn situation
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 2; k <= z + 2; k+=4) {
            for (int j = bottom; j <= bottom + 2; j+=2) {
                setBlockAt(i, j, k, SNOWLEAF);
            }
        }
    }
    //layer 2, 4, 6
    int mid = height + 3;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = mid; j <= mid+4; j+=2) {
             setBlockAt(i, j, z, SNOWLEAF);
        }
    }
    for (int i = z - 1; i <= z + 1; i++) {
        for (int j = mid; j <= mid+4; j+=2) {
            setBlockAt(x, j, i, SNOWLEAF);
        }
    }

}

void Chunk::drawSmolSnowy(int x, int z, int height) {
    // stem height 6
    for (int y = height; y < height + 6; y++) {
        if (y == height + 5) {
             setBlockAt(x, y, z, SNOWLEAF);
        } else {
             setBlockAt(x, y, z, DARKWOOD);
        }
    }
    //layer 3, 5
    int mid = height + 2;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = mid; j <= mid+2; j+=2) {
             setBlockAt(i, j, z, SNOWLEAF);
        }
    }
    for (int i = z - 1; i <= z + 1; i++) {
        for (int j = mid; j <= mid+2; j+=2) {
            setBlockAt(x, j, i, SNOWLEAF);
        }
    }
}

void Chunk::drawTallSnowy(int x, int z, int height) {
    // stem height 6
    for (int y = height; y < height + 7; y++) {
         setBlockAt(x, y, z, DARKWOOD);
    }
    //bottom
    for (int i = x - 1; i <= x + 1; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, height + 7, k, SNOWLEAF);
        }
    }
    //top
    setBlockAt(x, height + 8, z, SNOWLEAF);
}

void Chunk::drawLorgeAcacia(int x, int z, int height) {
    // stem height 4
    for (int y = height; y < height + 4; y++) {
         setBlockAt(x, y, z, ACACIA);
    }
    setBlockAt(x-1, height + 2, z, ACACIA);
    setBlockAt(x-2, height + 3, z, ACACIA);
    setBlockAt(x-3, height + 4, z, ACACIA);
    setBlockAt(x+1, height + 4, z, ACACIA);
    setBlockAt(x+2, height + 5, z, ACACIA);
    setBlockAt(x+3, height + 6, z, ACACIA);
    //bottom leaves on left
    for (int i = x - 6; i <= x-2; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, height + 5, k, ACACIALEAF);
        }
    }
    for (int i = x-5; i <= x-3; i++) {
        for (int k = z - 2; k <= z + 2; k += 4) {
            setBlockAt(i, height + 5, k, ACACIALEAF);
        }
    }
    //top leaves on left
    for (int i = x - 5; i <= x - 3; i++) {
       setBlockAt(i, height + 6, z, ACACIALEAF);
    }
    for (int k = z - 1; k <= z + 1; k++) {
       setBlockAt(x - 4, height + 6, k, ACACIALEAF);
    }
    //bottom leaves on right
    for (int i = x + 2; i <= x + 4; i++) {
        for (int j = z - 1; j <= z + 1; j++) {
            setBlockAt(i, height + 7, j, ACACIALEAF);
        }
     }
    //top leaves on right
    for (int i = x + 2; i <= x + 4; i++) {
       setBlockAt(i, height + 8, z, ACACIALEAF);
    }
    for (int k = z - 1; k <= z + 1; k++) {
       setBlockAt(x + 3, height + 8, k, ACACIALEAF);
    }
    //post trunk
    setBlockAt(x-3, height + 5, z, ACACIA);
    setBlockAt(x+3, height + 7, z, ACACIA);
}

void Chunk::drawSmolAcacia(int x, int z, int height) {
    // stem height 4
    for (int y = height; y < height + 4; y++) {
         setBlockAt(x, y, z, ACACIA);
    }
    setBlockAt(x+1, height + 4, z, ACACIA);
    //bottom leaf
    for (int i = x - 1; i <= x + 3; i++) {
        for (int k = z - 1; k <= z + 1; k++) {
            setBlockAt(i, height + 5, k, ACACIALEAF);
        }
    }
    for (int i = x; i <= x + 2; i++) {
        for (int k = z - 2; k <= z + 2; k+=4) {
            setBlockAt(i, height + 5, k, ACACIALEAF);
        }
    }
    //top cross
    for (int i = x; i <= x + 2; i++) {
       setBlockAt(i, height + 6, z, ACACIALEAF);
    }
    for (int j = z - 1; j <= z + 1; j++) {
       setBlockAt(x + 1, height + 6, j, ACACIALEAF);
    }
}

void Chunk::drawPalm(int x, int z, int height) {
    // stem height 4
    for (int y = height; y < height + 4; y++) {
         setBlockAt(x, y, z, PALM);
    }
    setBlockAt(x+1, height + 4, z, PALM);
    setBlockAt(x+1, height + 5, z, PALM);
    //bottom leaf layer
    setBlockAt(x+1, height + 6, z+3, PALMLEAF);
    setBlockAt(x, height + 6, z-4, PALMLEAF);
    setBlockAt(x-1, height + 6, z+2, PALMLEAF);
    setBlockAt(x-3, height + 6, z-2, PALMLEAF);
    setBlockAt(x+4, height + 6, z+1, PALMLEAF);
    setBlockAt(x+4, height + 6, z-3, PALMLEAF);
    //middle leaf layer
    setBlockAt(x+1, height + 7, z+2, PALMLEAF);
    setBlockAt(x, height + 7, z-3, PALMLEAF);
    setBlockAt(x-1, height + 7, z+1, PALMLEAF);
    setBlockAt(x-2, height + 7, z-2, PALMLEAF);
    setBlockAt(x+3, height + 7, z+1, PALMLEAF);
    setBlockAt(x+4, height + 7, z-2, PALMLEAF);
    for (int i = x; i <= x + 2; i++) {
        for (int k = z - 1; k <= z + 1; k++){
            setBlockAt(i, height + 7, k, PALMLEAF);
        }
    }
    setBlockAt(x-1, height + 7, z-1, PALMLEAF);
    setBlockAt(x, height + 7, z-2, PALMLEAF);
    setBlockAt(x+2, height + 7, z-2, PALMLEAF);
    setBlockAt(x+3, height + 7, z-1, PALMLEAF);
    //top leaf layer
    for (int i = x; i <= x + 2; i++) {
        for (int k = z - 1; k <= z; k++){
            setBlockAt(i, height + 8, k, PALMLEAF);
        }
    }
    //post trunk
    setBlockAt(x+1, height + 6, z, PALM);
    setBlockAt(x+1, height + 7, z, PALM);

}



//buildings look good when height roughly 100 and roughly 10x10

void Chunk::drawSpiky(int x_start, int x_end, int z_start, int z_end, int y, BlockType b) {
    int xgrad = 30;
    int xchange = 3;

    for(int x = x_start + 1; x <= x_end; x++) {
        int zgrad = 30;
        int zchange = 3;
        for(int z = z_start + 1; z <= z_end; z++) {
            if(x == x_start + (x_end-x_start)/2 + 1 && z == z_start + (z_end-z_start)/2 + 1) {
                xgrad += 3;
                zgrad += 3;
            }
            for(int height = y; height < y + xgrad + zgrad; height ++) {
                if ((x != x_start && x != x_end && z != z_start && z != z_end)) {
                    if(((x % 2 == 0) || (z % 2 == 0)) && height % 4 == 0) {
                        setBlockAt(x, height, z, WINDOW);
                    } else {
                        setBlockAt(x, height, z, b);
                    }
                }
            }
            if(x == x_start + (x_end-x_start)/2 + 1 && z == z_start + (z_end-z_start)/2 + 1) {
                xgrad -= 3;
                zgrad -= 3;
            }
            if(z <= z_start + (z_end-z_start)/2) {
                zgrad += 3;
                zchange += 3;
            }
            else {
                zgrad -= 3;
                zchange -= 3;
            }
        }
        if(x <= x_start + (x_end-x_start)/2) {
            xgrad += 3;
            xchange += 3;
        }
        else {
            xgrad -= 3;
            xchange -= 3;
        }
    }
}

void Chunk::drawBoring(int x_start, int x_end, int z_start, int z_end, int y, int height, BlockType b) {
    int base_ht = (3 * height) / 4;
    int top_ht = height / 4;
    int x_half = (x_end - x_start) / 2;
    int z_half = (z_end - z_start) / 2;

    for(int x = x_start ; x < x_end; x++) {
        for(int z = z_start ; z < z_end; z++) {
            for(int j = y; j < y + base_ht; j ++) {
                if ((x != x_start && x != x_end && z != z_start && z != z_end)) {
                    if(((x % 2 == 0) || (z % 2 == 0)) && j % 3 == 0) {
                        setBlockAt(x, j, z, WINDOW);
                    } else {
                        setBlockAt(x, j, z, b);
                    }
                }
            }
         }
    }
    int top = y + base_ht;
    int new_x = x_start + (x_half/2);
    int new_z = z_start + (z_half/2);
    for(int x = new_x; x < new_x + x_half; x++) {
        for(int z = new_z; z < new_z + z_half; z++) {
            for(int j = top; j < top + top_ht; j ++) {
                if ((x != x_start && x != x_end && z != z_start && z != z_end)) {
                    if(((x % 2 == 0) || (z % 2 == 0)) && j % 3 == 0) {
                        setBlockAt(x, j, z, WINDOW);
                    } else {
                        setBlockAt(x, j, z, b);
                    }
                }
            }
        }
    }

    for(int j = top + top_ht; j < top + top_ht + 6; j ++) {
        setBlockAt(x_start + x_half, j, z_start + z_half, b);
    }
}

void Chunk::drawSlanty(int x_start, int x_end, int z_start, int z_end, int y, int height, BlockType b) {
    //base
    for(int x = x_start ; x < x_end; x++) {
        for(int z = z_start ; z < z_end; z++) {
            for(int j = y; j < y + height; j++) {
                if ((x != x_start && x != x_end && z != z_start && z != z_end)) {
                    if(((x % 2 == 0) || (z % 2 == 0)) && j % 3 == 0) {
                        setBlockAt(x, j, z, WINDOW);
                    } else {
                        setBlockAt(x, j, z, b);
                    }
                }
            }
         }
    }
    int top = y + height;
    //diagonal part
    for (int x = x_start; x < x_end; x++) {
        int end = z_end - z_start;
        for(int z = z_start ; z < z_end; z++) {
            for(int j = top; j < top + end; j++) {
                if ((x != x_start && x != x_end && z != z_start && z != z_end)) {
                    if(((x % 2 == 0) || (z % 2 == 0)) && j % 3 == 0) {
                        setBlockAt(x, j, z, WINDOW);
                    } else {
                        setBlockAt(x, j, z, b);
                    }
                }
            }
            end--;
        }
    }
}

void Chunk::moveNPC(int npcId, Chunk* chunk){
    for (int i = 0; i < (int) npcs.size(); i++) {
        if (npcId == npcs[i]->id) {
            chunk->npcs.push_back(std::move(npcs[i]));
            npcs.erase(npcs.begin() + i);
            break;
        }
    }
}
