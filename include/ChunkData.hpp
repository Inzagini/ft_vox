#pragma once

#include "Mesh.hpp"
#include <memory>

enum class BlockType { AIR, SOLID };

struct Chunk {
    int block[16][256][16];
    std::vector<std::vector<int>> heightMap{16, std::vector<int>(16)};
    std::unique_ptr<Mesh> mesh;
};
