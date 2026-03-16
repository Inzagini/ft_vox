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
    bool hasMesh{false};

    ChunkSection() { std::memcpy(blocks, emptyChunkSection, sizeof(blocks)); }

    void reset() {
        std::memcpy(blocks, emptyChunkSection, sizeof(blocks));
        dirty = false;
        empty = true;
    }

    inline CubeType &getBlock(int x, int y, int z) { return blocks[x | (z << 4) | (y << 8)]; }
};

static ChunkSection emptyChunk_[16];

struct Chunk {
    ChunkSection sections[24];
    uint16_t heightMap[16][16];

    inline void reset() {
        for (auto &sec : sections)
            sec.reset();
    }

    inline CubeType &getBlock(int x, int y, int z) {
        int iy = y + 64;

        int sectionIndex = iy >> 4;
        int localY = iy & 15;
        return sections[sectionIndex].getBlock(x, localY, z);
    }
};
