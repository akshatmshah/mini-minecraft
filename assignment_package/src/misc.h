#pragma once

#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, BEDROCK, WOOD, ICE,
    SAND, LEAF, MUSHROOM, STEM, BROWNSHROOM, SNOWLEAF, WINDOW, CACTUS, OBSIDIAN, MYCELIUM,
    BLACK, PURPLE, BLUE, GRAY, WHITE, RED, ORANGE, YELLOW, BROWN, SNOWDIRT, REDSAND,
    ACACIA, ACACIALEAF, PALM, PALMLEAF, BIRCH, BIRCHLEAF, DARKWOOD, GRASSDEC,
    ROSE, DAISY, REDMUSH, BROWNMUSH, SHRUB
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

struct BlockFace {
    Direction d;
    std::array<glm::vec4, 4> vertexPos;
    glm::vec4 normal;

    BlockFace(Direction dir, glm::vec4 vert1, glm::vec4 vert2, glm::vec4 vert3, glm::vec4 vert4,
         glm::vec4 normal)
        : d(dir), vertexPos{vert1, vert2, vert3, vert4}, normal(normal) {}
};

struct BlockVertex {
    glm::vec4 pos;

    // relative UV coords that are offset based on BlockType
    glm::vec4 uv;

    BlockVertex(glm::vec4 p, glm::vec4 u)
        : pos(p), uv(u) {}
};

struct BlockNeighbor {
    Direction dir;
    glm::vec3 offset;
    std::array<BlockVertex, 4> vertices;    // vertex set to make a quadrangle facing in this direction
    BlockNeighbor(Direction direction, glm::vec3 off,
                  BlockVertex a,  BlockVertex b, BlockVertex c,  BlockVertex d)
        : dir(direction), offset(off), vertices{a, b, c, d}
    {}
};

class Misc
{
public:
    Misc();
};

