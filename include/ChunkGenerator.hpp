#pragma once

#include "ChunkData.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include "glm/glm.hpp"
#include <algorithm>
#include <iostream>

class ChunkGenerator {

  public:
    ChunkGenerator(unsigned int seed) : terrainNoise(seed) {};
    void generateChunk(Chunk &chunk, const int chunkX, const int chunkZ);

  private:
    PerlinNoise terrainNoise;
    const float scale{0.01f};
    const float heightMultiplier{50.0f};
    const float baseHeight{50.0f};
    const int chunkSize{16};
    unsigned int seed;
};
