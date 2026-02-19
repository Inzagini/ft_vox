#pragma once

#include "CubeData.hpp"
#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include "glm/glm.hpp"

class ChunkGenerator {

  public:
    ChunkGenerator(unsigned int seed) : terrainNoise(seed) {};
    tCHUNK generate();

  private:
    void addFace(glm::vec3 &pos, int face, tCHUNK &chunk, int &vertexOffset);

  private:
    PerlinNoise terrainNoise;
    const float scale{0.01f};
    const float heightMultiplier{50.0f};
    const int chunkSize{16};
    unsigned int seed;
    std::vector<std::vector<int>> heightMap{16, std::vector<int>(16)};
};
