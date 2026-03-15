#pragma once

#include "ChunkData.hpp"
#include "PerlinNoise.hpp"
#include <iostream>

class ChunkGenerator {

  public:
    ChunkGenerator(unsigned int seed) : terrainNoise(seed) {};
    void generateChunk(Chunk &chunk, const int chunkX, const int chunkZ);

  private:
    PerlinNoise terrainNoise;
    static constexpr float scale{0.01f};
    static constexpr float heightMultiplier{50.0f};
    static constexpr float baseHeight{25.0f};
    static constexpr uint8_t chunkSize{16};
    unsigned int seed;
};
