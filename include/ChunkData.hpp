#pragma once

#include "CubeData.hpp"
#include "Mesh.hpp"
#include <cstring>
#include <memory>

static CubeType emptyChunk[16 * 16 * 256];

struct Chunk {
    CubeType blocks[16 * 16 * 256];
    uint16_t heightMap[16][16];
    Mesh mesh;
    bool hasMesh{false};
    bool dirty{false};

    Chunk() { std::memcpy(blocks, emptyChunk, sizeof(blocks)); }

    inline void reset() {
        std::memcpy(blocks, emptyChunk, sizeof(blocks));
        std::memset(heightMap, 0, sizeof(heightMap));
        hasMesh = false;
        dirty = false;
    }

    inline CubeType &getBlock(int x, int y, int z) { return blocks[x | (z << 4) | (y << 8)]; }
};
