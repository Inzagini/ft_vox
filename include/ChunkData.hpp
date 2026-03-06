#pragma once

#include "CubeData.hpp"
#include "Mesh.hpp"
#include <memory>

struct ChunkData {
    CubeType block[16][16][256];
    int heightMap[16][16];
};

struct Chunk {
    CubeType blocks[16 * 16 * 256];
    uint16_t heightMap[16][16];
    // std::unique_ptr<Mesh> mesh = nullptr;
    Mesh mesh;
    bool hasMesh{false};
    bool dirty{true};

    inline CubeType &getBlock(int x, int y, int z) { return blocks[x | (z << 4) | (y << 8)]; }
};
