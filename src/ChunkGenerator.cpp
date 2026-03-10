#include "ChunkGenerator.hpp"
#include <algorithm>
#include <cstdlib>

void ChunkGenerator::generateChunk(Chunk &chunk, const int chunkX, const int chunkZ) {
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = (chunkX * chunkSize + x) * scale;
            float worldZ = (chunkZ * chunkSize + z) * scale;
            int hillHeight = std::floor(terrainNoise.octaveNoise(worldX, worldZ, 6, 0.5, 2.0) *
                                        heightMultiplier);

            float mountainMask =
                terrainNoise.octaveNoise(worldX / 4.0f, worldZ / 4.0f, 3, 0.5, 2.0);
            mountainMask = std::clamp(mountainMask, 0.0f, 1.0f);
            mountainMask = std::pow(mountainMask, 0.7f) * 20; // smooth transition, rarer mountains

            float c = 2.0f;
            float moutainHeight =
                std::pow(terrainNoise.octaveNoise(worldX / c, worldZ / c, 4, 0.5, 2.0), 4.0f);

            int sum = floor(baseHeight + hillHeight + moutainHeight);

            int height = sum > 255 ? 255 : sum;
            chunk.heightMap[x][z] = height;

            for (int y = 0; y < 256; y++) {
                if (y == height)
                    chunk.getBlock(x, y, z) = CubeType::GRASS;
                else if (y >= height - 3 && y < height)
                    chunk.getBlock(x, y, z) = CubeType::DIRT;
                else if (y < height)
                    chunk.getBlock(x, y, z) = CubeType::STONE;
                else {
                    chunk.getBlock(x, y, z) = CubeType::AIR;
                }
            }
        }
    }
}
