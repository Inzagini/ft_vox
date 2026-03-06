#include "ChunkGenerator.hpp"

void ChunkGenerator::generateChunk(Chunk &chunk, const int chunkX, const int chunkZ) {
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = (chunkX * chunkSize + x) * scale;
            float worldZ = (chunkZ * chunkSize + z) * scale;
            int height = std::floor(terrainNoise.octaveNoise(worldX, worldZ, 6, 0.5, 2.0) *
                                    heightMultiplier);

            height += baseHeight;
            height = height > 255 ? 255 : height;
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
