#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"

tCHUNK ChunkGenerator::generate(const int chunkX, const int chunkZ) {
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            float worldX = (chunkX * chunkSize + x) * scale;
            float worldZ = (chunkZ * chunkSize + z) * scale;
            float height = std::floor(terrainNoise.octaveNoise(worldX, worldZ, 6, 0.5, 2.0) *
                                      heightMultiplier);
            height += baseHeight;
            heightMap[x][z] = height;
        }
    }

    tCHUNK chunk;
    int vertexOffset{};

    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            for (int y = 0; y <= heightMap[x][z]; y++) {
                glm::vec3 pos(chunkX * chunkSize + x - 1.0f, // world X
                              y,
                              chunkZ * chunkSize + z - 1.0f // world Z
                );
                if (y == heightMap[x][z]) // top
                    addFace(pos, (int)CubeFace::TOP, chunk, vertexOffset);

                if (y == 0) // bottom
                    addFace(pos, (int)CubeFace::BOTTOM, chunk, vertexOffset);

                if (x == 0 || y > heightMap[x - 1][z]) // left
                    addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);

                if (x == chunkSize - 1 || y > heightMap[x + 1][z]) // right
                    addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);

                if (z == 0 || y > heightMap[x][z - 1]) // front
                    addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);

                if (z == chunkSize - 1 || y > heightMap[x][z + 1]) // back
                    addFace(pos, (int)CubeFace::BACK, chunk, vertexOffset);
            }
        }
    }
    return chunk;
}

void ChunkGenerator::addFace(glm::vec3 &pos, int face, tCHUNK &chunk, int &vertexOffset) {
    for (int i = 0; i < 4; i++) {
        chunk.vertices.push_back(cubeFaceVertices[face][i].x + pos.x);
        chunk.vertices.push_back(cubeFaceVertices[face][i].y + pos.y);
        chunk.vertices.push_back(cubeFaceVertices[face][i].z + pos.z);
    }
    for (int i = 0; i < 6; i++) {
        chunk.indices.push_back(cubeFaceIndices[i] + vertexOffset);
    }
    vertexOffset += 4; // 4 vertices per face
}
