#pragma once

#include "CubeData.hpp"
#include "Mesh.hpp"
#include <memory>

struct Chunk {
    CubeType block[16][256][16];
    int heightMap[16][16];
    // std::vector<std::vector<int>> heightMap{16, std::vector<int>(16)};
    std::unique_ptr<Mesh> mesh = nullptr;
    bool dirty{false};
};
