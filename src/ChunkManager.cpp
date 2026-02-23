#include "ChunkManager.hpp"
#include "CubeData.hpp"
#include <memory>

void ChunkManager::update(const glm::vec3 &pos) {
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);
    meshing(chunkX, chunkZ);
    unload(chunkX, chunkZ);
}

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -renderDistance; dx <= renderDistance; dx++) {
        for (int dz = -renderDistance; dz <= renderDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> key = {chunkX, chunkZ};

            if (!activeChunk.contains(key)) {
                Chunk chunk;
                generator.generateChunk(chunk, chunkX, chunkZ);

                if (!activeChunk.contains({chunkX - 1, chunkZ}) ||
                    !activeChunk.contains({chunkX + 1, chunkZ}) ||
                    !activeChunk.contains({chunkX, chunkZ - 1}) ||
                    !activeChunk.contains({chunkX, chunkZ + 1}))
                    chunk.dirty = true;

                activeChunk[key] = std::move(chunk);
                markNeigborChunkDirty(chunkX, chunkZ);
            }
        }
    }
}

void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    for (auto it = activeChunk.begin(); it != activeChunk.end();) {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;
        if (abs(chunkX - playerChunkX) > renderDistance ||
            abs(chunkZ - playerChunkZ) > renderDistance) {
            it = activeChunk.erase(it); // safely remove outside chunk
        } else {
            ++it;
        }
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ) {

    for (auto &[key, chunk] : activeChunk) {
        if (chunk.mesh == nullptr || chunk.dirty) {
            addFaces(chunk, key.first, key.second);
            chunk.dirty = false;
        }
    }
}

void ChunkManager::render(Shader &shader) {

    for (auto &[key, chunk] : activeChunk) {
        glm::mat4 model = glm::mat4(1.0f);

        // Offset the chunk in world space (assuming chunk size = 16)
        model = glm::translate(model, glm::vec3(key.first, 0.0f, key.second));
        shader.setMat4("model", model);
        chunk.mesh->draw();
    }
}

void ChunkManager::addFaces(Chunk &_chunk, const int chunkX, const int chunkZ) {

    tCHUNK chunk;
    int vertexOffset{};
    int chunkSize = 16;

    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            for (int y = 0; y <= _chunk.heightMap[x][z] + 1; y++) {
                glm::vec3 pos(chunkX * (chunkSize - 1) + x, // world X
                              y,
                              chunkZ * (chunkSize - 1) + z // world Z
                );

                if (_chunk.block[x][y][z] != CubeType::SOLID)
                    continue;

                if (_chunk.block[x][y + 1][z] == CubeType::AIR) // top
                    addFace(pos, (int)CubeFace::TOP, chunk, vertexOffset);

                if (y != 0 && _chunk.block[x][y - 1][z] == CubeType::AIR) // bottom
                    addFace(pos, (int)CubeFace::BOTTOM, chunk, vertexOffset);

                if (x > 0 && _chunk.block[x - 1][y][z] == CubeType::AIR) // left
                    addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);

                if (x < chunkSize - 1 && _chunk.block[x + 1][y][z] == CubeType::AIR) // right
                    addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);

                if (z > 0 && _chunk.block[x][y][z - 1] == CubeType::AIR) // front
                    addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);

                if (z < chunkSize - 1 && _chunk.block[x][y][z + 1] == CubeType::AIR) // back
                    addFace(pos, (int)CubeFace::BACK, chunk, vertexOffset);

                if (x == 0) { // left on border
                    std::pair<int, int> key{chunkX - 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[15][y][z] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);
                    }
                }

                if (x == chunkSize - 1) { // right on border
                    std::pair<int, int> key{chunkX + 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[0][y][z] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);
                    }
                }

                if (z == 0) { // front on border
                    std::pair<int, int> key{chunkX, chunkZ - 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][y][15] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);
                    }
                }

                if (z == chunkSize - 1) { // back on border
                    std::pair<int, int> key{chunkX, chunkZ + 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][y][0] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::BACK, chunk, vertexOffset);
                    }
                }
            }
        }
    }
    _chunk.mesh = std::make_unique<Mesh>(chunk, 3, GL_STATIC_DRAW);
}

void ChunkManager::addFace(glm::vec3 &pos, int face, tCHUNK &chunk, int &vertexOffset) {
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

void ChunkManager::markNeigborChunkDirty(const int chunkX, const int chunkZ) {
    std::pair<int, int> neighbor[4] = {
        {chunkX - 1, chunkZ}, {chunkX + 1, chunkZ}, {chunkX, chunkZ - 1}, {chunkX, chunkZ + 1}};

    for (auto &key : neighbor) {
        if (activeChunk.contains(key))
            activeChunk[key].dirty = true;
    }
}
