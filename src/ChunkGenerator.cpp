#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include <algorithm>
#include <cstdlib>
#include <memory>

void ChunkGenerator::generateChunk(Chunk &chunk, const int chunkX, const int chunkZ) {

    const float worldOffsetX = chunkX * chunkSize * scale;
    const float worldOffsetZ = chunkZ * chunkSize * scale;

    for (int z = 0; z < chunkSize; z++) {
        float worldZ = worldOffsetZ + z * scale;
        for (int x = 0; x < chunkSize; x++) {
            float worldX = worldOffsetX + x * scale;
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

            chunk.sections[0].getBlock(x, 0, z) = CubeType::BEDROCK;

            for (int y = -63; y <= height; y++) {
                CubeType type;

                if (y < height - 3)
                    type = CubeType::STONE;
                else if (y < height)
                    type = CubeType::DIRT;
                else if (y == height)
                    type = CubeType::GRASS;

                int iy = y + 64;
                int sectionIndex = iy >> 4; // divide by 16
                int localY = iy & 15;       // modulo 16

                ChunkSection &section = chunk.sections[sectionIndex];

                section.getBlock(x, localY, z) = type;

                if (type != CubeType::AIR) {
                    section.empty = false;
                    section.dirty = true;
                }
            }
        }
    }
}
