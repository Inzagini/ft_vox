#pragma once

#include "CubeData.hpp"
#include "Mesh.hpp"
#include <cstring>
#include <memory>

static constexpr CubeType emptyChunk[16 * 16 * 384] = {};
static constexpr CubeType emptyChunkSection[16 * 16 * 16] = {};

struct ChunkSection {
    CubeType blocks[16 * 16 * 16];
    Mesh mesh;
    bool dirty{false};
    bool empty{true};

    ChunkSection() { std::memcpy(blocks, emptyChunkSection, sizeof(blocks)); }

    void reset() {
        std::memcpy(blocks, emptyChunkSection, sizeof(blocks));
        dirty = false;
        empty = true;
    }
};

static ChunkSection emptyChunk_[16];

struct Chunk {
    ChunkSection sections[16];
    CubeType blocks[16 * 16 * 384];
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

    inline CubeType &getBlock(int x, int y, int z) {
        return blocks[x | (z << 4) | ((y + 64) << 8)];
    }
};
