#include "ChunkManager.hpp"

void ChunkManager::update(Camera &camera) {
    glm::vec3 pos = camera.getPos();
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);
    meshing(chunkX, chunkZ, camera);
    unload(chunkX, chunkZ);
}

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -loadDistance; dx <= loadDistance; dx++) {
        for (int dz = -loadDistance; dz <= loadDistance; dz++) {
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

                activeChunk.try_emplace(key, std::move(chunk));
                markNeigborChunkDirty(chunkX, chunkZ);
            }
        }
    }
}

void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    for (auto it = activeChunk.begin(); it != activeChunk.end();) {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;
        if (abs(chunkX - playerChunkX) > loadDistance ||
            abs(chunkZ - playerChunkZ) > loadDistance) {
            it = activeChunk.erase(it); // safely remove outside chunk
        } else {
            ++it;
        }
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ, Camera &camera) {

    for (auto &[key, chunk] : activeChunk) {
        if ((chunk.mesh == nullptr || chunk.dirty) && camera.isInFOV(key.first, key.second)) {
            addFaces(chunk, key.first, key.second);
            chunk.dirty = false;
        }
    }
}

void ChunkManager::render(Shader &shader, const glm::vec3 &playerPos, Camera &camera) {

    const int playerChunkX = floor(playerPos.x / 16);
    const int playerChunkZ = floor(playerPos.z / 16);

    for (auto &[key, chunk] : activeChunk) {

        const int relativeChunkX = key.first - playerChunkX;
        const int relativeChunkZ = key.second - playerChunkZ;
        const glm::vec3 chunkPos =
            glm::vec3(relativeChunkX * chunkSize, 0.0f, relativeChunkZ * chunkSize);

        if (abs(relativeChunkX) <= renderDistance && abs(relativeChunkZ) <= renderDistance &&
            camera.isInFOV(key.first, key.second)) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPos);
            shader.setMat4("model", model);
            chunk.mesh->draw();
        }
    }
}
void ChunkManager::addFaces(Chunk &_chunk, const int chunkX, const int chunkZ) {

    tCHUNK chunk;
    int vertexOffset{};
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            for (int y = 0; y < 256; y++) {
                glm::vec3 pos(x, y, z);

                if (_chunk.block[x][z][y] != CubeType::SOLID)
                    continue;

                if (_chunk.block[x][z][y + 1] == CubeType::AIR) // top
                    addFace(pos, (int)CubeFace::TOP, chunk, vertexOffset);

                if (y != 0 && _chunk.block[x][z][y - 1] == CubeType::AIR) // bottom
                    addFace(pos, (int)CubeFace::BOTTOM, chunk, vertexOffset);

                if (x > 0 && _chunk.block[x - 1][z][y] == CubeType::AIR) // left
                    addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);

                if (x < chunkSize - 1 && _chunk.block[x + 1][z][y] == CubeType::AIR) // right
                    addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);

                if (z > 0 && _chunk.block[x][z - 1][y] == CubeType::AIR) // front
                    addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);

                if (z < chunkSize - 1 && _chunk.block[x][z + 1][y] == CubeType::AIR) // back
                    addFace(pos, (int)CubeFace::BACK, chunk, vertexOffset);

                if (x == 0) { // left on border
                    std::pair<int, int> key{chunkX - 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[15][z][y] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::LEFT, chunk, vertexOffset);
                    }
                }

                if (x == chunkSize - 1) { // right on border
                    std::pair<int, int> key{chunkX + 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[0][z][y] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::RIGHT, chunk, vertexOffset);
                    }
                }

                if (z == 0) { // front on border
                    std::pair<int, int> key{chunkX, chunkZ - 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][15][y] == CubeType::AIR)
                            addFace(pos, (int)CubeFace::FRONT, chunk, vertexOffset);
                    }
                }

                if (z == chunkSize - 1) { // back on border
                    std::pair<int, int> key{chunkX, chunkZ + 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][0][y] == CubeType::AIR)
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
