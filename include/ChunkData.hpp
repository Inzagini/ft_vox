#pragma once

#include "Mesh.hpp"
#include <memory>

enum class BlockType { AIR, SOLID };

struct Chunk {
    int block[16][256][16];
    std::unique_ptr<Mesh> mesh;
};
