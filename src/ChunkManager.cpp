#include "ChunkManager.hpp"
#include "ChunkData.hpp"
#include "Window.hpp"
#include <memory>
#include <mutex>

void ChunkManager::update(Camera &camera) {
    glm::vec3 pos = camera.getPos();
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);
    meshing(chunkX, chunkZ, camera);
    uploadMesh();
    unload(chunkX, chunkZ);
}

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -loadDistance; dx <= loadDistance; dx++) {
        for (int dz = -loadDistance; dz <= loadDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> key = {chunkX, chunkZ};

            {
                std::lock_guard<std::mutex> lock(activeChunkMutex);
                if (activeChunk.contains(key))
                    continue;
            }

            threadPool.enqueue([this, chunkX, chunkZ, key] {
                Chunk chunk;
                generator.generateChunk(chunk, chunkX, chunkZ);

                bool dirty = false;
                {
                    std::lock_guard<std::mutex> lock(activeChunkMutex);
                    if (!activeChunk.contains({chunkX - 1, chunkZ}) ||
                        !activeChunk.contains({chunkX + 1, chunkZ}) ||
                        !activeChunk.contains({chunkX, chunkZ - 1}) ||
                        !activeChunk.contains({chunkX, chunkZ + 1}))
                        dirty = true;
                }
                chunk.dirty = dirty;
                {
                    std::lock_guard<std::mutex> lock(activeChunkMutex);
                    activeChunk.try_emplace(key, std::move(chunk));
                    if (chunk.dirty)
                        markNeigborChunkDirty(chunkX, chunkZ);
                }
            });
        }
    }
}

void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    std::vector<std::pair<int, int>> chunkToUnload;

    // for (auto it = activeChunk.begin(); it != activeChunk.end(); it++) {
    //     int chunkX = it->first.first;
    //     int chunkZ = it->first.second;
    //     if (abs(chunkX - playerChunkX) > loadDistance ||
    //         abs(chunkZ - playerChunkZ) > loadDistance) {
    //         it = activeChunk.erase(it); // safely remove outside chunk
    //     }
    // }

    {
        std::lock_guard<std::mutex> lock(activeChunkMutex);
        for (auto it = activeChunk.begin(); it != activeChunk.end(); it++) {
            int chunkX = it->first.first;
            int chunkZ = it->first.second;
            if (abs(chunkX - playerChunkX) > loadDistance ||
                abs(chunkZ - playerChunkZ) > loadDistance) {
                chunkToUnload.push_back({chunkX, chunkZ});
            }
        }
    }

    for (auto &[chunkX, chunkZ] : chunkToUnload) {
        threadPool.enqueue([this, chunkX, chunkZ] {
            {
                std::lock_guard<std::mutex> lock(activeChunkMutex);
                activeChunk.erase({chunkX, chunkZ});
            }
        });
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ, Camera &camera) {
    std::vector<std::pair<std::pair<int, int>, Chunk *>> chunkToMesh;

    // for (auto &[key, chunk] : activeChunk) {
    //     if ((chunk.mesh == nullptr || chunk.dirty) && camera.isInFOV(key.first, key.second)) {
    //         addFaces(chunk, key.first, key.second);
    //         chunk.dirty = false;
    //     }
    // }

    {
        std::lock_guard<std::mutex> lock(activeChunkMutex);
        for (auto &[key, chunk] : activeChunk) {
            if ((chunk.mesh == nullptr || chunk.dirty) && camera.isInFOV(key.first, key.second))
                chunkToMesh.push_back({key, &chunk});
        }
    }
    for (auto &[key, chunkPtr] : chunkToMesh) {
        threadPool.enqueue([this, key, chunkPtr] { addFaces(*chunkPtr, key.first, key.second); });
    }
}

void ChunkManager::uploadMesh() {
    std::lock_guard<std::mutex> lock(uploadQMutex);

    while (!uploadQ.empty()) {
        auto item = std::move(uploadQ.back());
        uploadQ.pop_back();

        std::lock_guard<std::mutex> chunkLock(activeChunkMutex);

        if (activeChunk.contains(item.first)) {
            activeChunk[item.first].mesh = std::make_unique<Mesh>(item.second, 5, GL_STATIC_DRAW);
            activeChunk[item.first].dirty = false;
        }
    }
}

void ChunkManager::render(Shader &shader, const glm::vec3 &playerPos, Camera &camera) {

    const int playerChunkX = floor(playerPos.x / 16);
    const int playerChunkZ = floor(playerPos.z / 16);

    std::vector<std::pair<glm::vec3, Mesh *>> chunkTorender;

    // for (auto &[key, chunk] : activeChunk) {
    //
    //     const int relativeChunkX = key.first - playerChunkX;
    //     const int relativeChunkZ = key.second - playerChunkZ;
    //     const glm::vec3 chunkPos =
    //         glm::vec3(relativeChunkX * chunkSize, 0.0f, relativeChunkZ * chunkSize);
    //
    //     if (abs(relativeChunkX) <= renderDistance && abs(relativeChunkZ) <= renderDistance &&
    //         camera.isInFOV(key.first, key.second)) {
    //         glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPos);
    //         shader.setMat4("model", model);
    //         chunk.mesh->draw();
    //     }
    // }

    {
        std::lock_guard<std::mutex> lock(activeChunkMutex);
        for (auto &[key, chunk] : activeChunk) {

            const int relativeChunkX = key.first - playerChunkX;
            const int relativeChunkZ = key.second - playerChunkZ;
            const glm::vec3 chunkPos =
                glm::vec3(relativeChunkX * chunkSize, 0.0f, relativeChunkZ * chunkSize);

            if (abs(relativeChunkX) <= renderDistance && abs(relativeChunkZ) <= renderDistance &&
                camera.isInFOV(key.first, key.second)) {
                if (chunk.mesh.get())
                    chunkTorender.push_back({chunkPos, chunk.mesh.get()});
            }
        }
    }

    for (auto &[pos, mesh] : chunkTorender) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        mesh->draw();
    }
}
void ChunkManager::addFaces(Chunk &_chunk, const int chunkX, const int chunkZ) {

    tMesh mesh;
    int vertexOffset{};
    for (int z = 0; z < chunkSize; z++) {
        for (int x = 0; x < chunkSize; x++) {
            for (int y = 0; y < 256; y++) {
                glm::vec3 pos(x, y, z);

                if (_chunk.block[x][z][y] == CubeType::AIR)
                    continue;

                if (_chunk.block[x][z][y + 1] == CubeType::AIR) // top
                    addFace(pos, _chunk.block[x][z][y], CubeFace::TOP, mesh, vertexOffset);

                if (y != 0 && _chunk.block[x][z][y - 1] == CubeType::AIR) // bottom
                    addFace(pos, _chunk.block[x][z][y], CubeFace::BOTTOM, mesh, vertexOffset);

                if (x > 0 && _chunk.block[x - 1][z][y] == CubeType::AIR) // left
                    addFace(pos, _chunk.block[x][z][y], CubeFace::LEFT, mesh, vertexOffset);

                if (x < chunkSize - 1 && _chunk.block[x + 1][z][y] == CubeType::AIR) // right
                    addFace(pos, _chunk.block[x][z][y], CubeFace::RIGHT, mesh, vertexOffset);

                if (z > 0 && _chunk.block[x][z - 1][y] == CubeType::AIR) // front
                    addFace(pos, _chunk.block[x][z][y], CubeFace::FRONT, mesh, vertexOffset);

                if (z < chunkSize - 1 && _chunk.block[x][z + 1][y] == CubeType::AIR) // back
                    addFace(pos, _chunk.block[x][z][y], CubeFace::BACK, mesh, vertexOffset);

                if (x == 0) { // left on border
                    std::pair<int, int> key{chunkX - 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[15][z][y] == CubeType::AIR)
                            addFace(pos, _chunk.block[x][z][y], CubeFace::LEFT, mesh, vertexOffset);
                    }
                }

                if (x == chunkSize - 1) { // right on border
                    std::pair<int, int> key{chunkX + 1, chunkZ};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[0][z][y] == CubeType::AIR)
                            addFace(pos, _chunk.block[x][z][y], CubeFace::RIGHT, mesh,
                                    vertexOffset);
                    }
                }

                if (z == 0) { // front on border
                    std::pair<int, int> key{chunkX, chunkZ - 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][15][y] == CubeType::AIR)
                            addFace(pos, _chunk.block[x][z][y], CubeFace::FRONT, mesh,
                                    vertexOffset);
                    }
                }

                if (z == chunkSize - 1) { // back on border
                    std::pair<int, int> key{chunkX, chunkZ + 1};

                    if (activeChunk.contains(key)) {
                        Chunk &neighborChunk = activeChunk[key];

                        if (neighborChunk.block[x][0][y] == CubeType::AIR)
                            addFace(pos, _chunk.block[x][z][y], CubeFace::BACK, mesh, vertexOffset);
                    }
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(uploadQMutex);
        uploadQ.push_back({{chunkX, chunkZ}, std::move(mesh)});
    }
}

void ChunkManager::addFace(const glm::vec3 &pos, const CubeType &type, CubeFace face, tMesh &chunk,
                           int &vertexOffset) {

    uint8_t iface = static_cast<uint8_t>(face);
    glm::vec2 tile = texture.getTextureUV(type, face);
    glm::vec2 tileSize = Atlas::getTileSize();

    glm::vec2 uv0 = tile;
    glm::vec2 uv1 = tile + glm::vec2(tileSize.x, 0.0f);
    glm::vec2 uv2 = tile + glm::vec2(tileSize.x, tileSize.y);
    glm::vec2 uv3 = tile + glm::vec2(0.0f, tileSize.y);
    glm::vec2 faceUV[4] = {uv0, uv1, uv2, uv3};

    for (int i = 0; i < 4; i++) {
        chunk.vertices.push_back(cubeFaceVertices[iface][i].x + pos.x);
        chunk.vertices.push_back(cubeFaceVertices[iface][i].y + pos.y);
        chunk.vertices.push_back(cubeFaceVertices[iface][i].z + pos.z);

        chunk.vertices.push_back(faceUV[i].x);
        chunk.vertices.push_back(faceUV[i].y);
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
