#pragma once

#include "Mesh.hpp"
#include <memory>

struct Chunk {
    int block[16][256][16];
    std::unique_ptr<Mesh> mesh;
};
