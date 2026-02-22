#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"

void ChunkGenerator::generateChunk(Chunk &chunk, const int chunkX, const int chunkZ) {
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = (chunkX * chunkSize + x) * scale;
            float worldZ = (chunkZ * chunkSize + z) * scale;
            float height = std::floor(terrainNoise.octaveNoise(worldX, worldZ, 6, 0.5, 2.0) *
                                      heightMultiplier);
            height += baseHeight;
            chunk.heightMap[x][z] = height;
        }
    }
}
