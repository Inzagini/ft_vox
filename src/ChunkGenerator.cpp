#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"
#include <iostream>

tCHUNK ChunkGenerator::generate() {
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            float height = std::floor(terrainNoise.octaveNoise(x * scale, z * scale, 6, 0.5, 2.0) *
                                      heightMultiplier);
            heightMap[x][z] = height;
        }
    }

    tCHUNK chunk;
    int vertexOffset{};

    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            for (int y = 0; y <= heightMap[x][z]; y++) {
                bool top = (y == heightMap[x][z]);
                bool bottom = (y == 0);
                bool left = (x == 0 || y > heightMap[x - 1][z]);
                bool right = (x == chunkSize - 1 || y > heightMap[x + 1][z]);
                bool front = (z == 0 || y > heightMap[x][z - 1]);
                bool back = (z == chunkSize - 1 || y > heightMap[x][z + 1]);

                glm::vec3 pos(x, y, z);
                if (top)
                    addFace(pos, (int)CubeFace::TOP, chunk, vertexOffset);

                if (bottom)
                    addFace(pos, (int)CubeFace::BOTTOM, chunk, vertexOffset);

                if (left)
                    addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);

                if (right)
                    addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);

                if (front)
                    addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);

                if (back)
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
