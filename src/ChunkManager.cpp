#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "CubeData.hpp"
#include "Texture.hpp"
#include <mutex>

void ChunkManager::update(Camera &camera) {
    glm::vec3 pos = camera.getPos();
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);
    meshing(chunkX, chunkZ, camera);
    unload(chunkX, chunkZ);
}

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    std::vector<std::pair<uint64_t, Chunk>> generatedChunks;
    generatedChunks.reserve(700);

    Chunk chunk;
    for (int dx = -_loadDistance; dx <= _loadDistance; dx++) {
        for (int dz = -_loadDistance; dz <= _loadDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            uint64_t key = makeHash(chunkX, chunkZ);

            if (_activeChunk.contains(key))
                continue;

            generator.generateChunk(chunk, chunkX, chunkZ);
            generatedChunks.push_back({std::move(key), std::move(chunk)});
            chunk.reset();
        }
    }

    while (!generatedChunks.empty()) {

        auto [key, chunk] = std::move(generatedChunks.back());
        generatedChunks.pop_back();

        int chunkX = getXFromHash(key);
        int chunkZ = getZFromHash(key);

        _activeChunk.try_emplace(std::move(key), std::move(chunk));
        markNeigborChunkDirty(chunkX, chunkZ);
    }
}

void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {

    {
        for (auto it = _activeChunk.begin(); it != _activeChunk.end();) {
            int chunkX = getXFromHash(it->first);
            int chunkZ = getZFromHash(it->first);
            if (abs(chunkX - playerChunkX) > _loadDistance ||
                abs(chunkZ - playerChunkZ) > _loadDistance) {
                it = _activeChunk.erase(it); // safely remove outside chunk
            } else {
                ++it;
            }
        }
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ, Camera &camera) {
    {
        // std::lock_guard<std::mutex> lock(activeChunkMutex);
        tMesh tmesh;
        for (auto &[key, chunk] : _activeChunk) {
            int chunkX = getXFromHash(key);
            int chunkZ = getZFromHash(key);
            if ((chunk.hasMesh == false || chunk.dirty) && camera.isInFOV(chunkX, chunkZ)) {

                addFaces(chunk, chunkX, chunkZ, tmesh);
                chunk.mesh.make(tmesh, 5);
                chunk.hasMesh = true;
                chunk.dirty = false;
                tmesh.reset();
            }
        }
    }
}

void ChunkManager::render(Shader &shader, const glm::vec3 &playerPos, Camera &camera) {

    const int playerChunkX = floor(playerPos.x / 16);
    const int playerChunkZ = floor(playerPos.z / 16);

    for (auto &[key, chunk] : _activeChunk) {

        if (chunk.hasMesh == false)
            continue;

        const int chunkX = getXFromHash(key);
        const int chunkZ = getZFromHash(key);

        const int relativeChunkX = chunkX - playerChunkX;
        const int relativeChunkZ = chunkZ - playerChunkZ;
        const glm::vec3 chunkPos =
            glm::vec3(relativeChunkX * _chunkSize, 0.0f, relativeChunkZ * _chunkSize);

        if (abs(relativeChunkX) <= _renderDistance && abs(relativeChunkZ) <= _renderDistance &&
            camera.isInFOV(chunkX, chunkZ)) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPos);
            shader.setMat4("model", model);
            chunk.mesh.draw();
        }
    }
}
void ChunkManager::addFaces(Chunk &chunk, const int chunkX, const int chunkZ, tMesh &mesh) {

    int vertexOffset{};
    for (int z = 0; z < _chunkSize; z++) {
        for (int x = 0; x < _chunkSize; x++) {
            for (int y = 0; y < 256; y++) {
                glm::vec3 pos(x, y, z);

                if (chunk.getBlock(x, y, z) == CubeType::AIR)
                    continue;

                if (chunk.getBlock(x, y + 1, z) == CubeType::AIR) // top
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::TOP, mesh, vertexOffset);

                if (y != 0 && chunk.getBlock(x, y - 1, z) == CubeType::AIR) // bottom
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::BOTTOM, mesh, vertexOffset);

                if (x > 0 && chunk.getBlock(x - 1, y, z) == CubeType::AIR) // left
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::LEFT, mesh, vertexOffset);

                if (x < _chunkSize - 1 && chunk.getBlock(x + 1, y, z) == CubeType::AIR) // right
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::RIGHT, mesh, vertexOffset);

                if (z > 0 && chunk.getBlock(x, y, z - 1) == CubeType::AIR) // front
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::FRONT, mesh, vertexOffset);

                if (z < _chunkSize - 1 && chunk.getBlock(x, y, z + 1) == CubeType::AIR) // back
                    addFace(pos, chunk.getBlock(x, y, z), CubeFace::BACK, mesh, vertexOffset);

                if (x == 0) { // left on border
                    uint64_t key = makeHash(chunkX - 1, chunkZ);

                    auto it = _activeChunk.find(key);
                    if (it != _activeChunk.end()) {
                        Chunk &neighborChunk = it->second;

                        if (neighborChunk.getBlock(_chunkSize - 1, y, z) == CubeType::AIR)
                            addFace(pos, chunk.getBlock(x, y, z), CubeFace::LEFT, mesh,
                                    vertexOffset);
                    }
                }

                if (z == 0) { // front on border
                    uint64_t key = makeHash(chunkX, chunkZ - 1);
                    auto it = _activeChunk.find(key);
                    if (it != _activeChunk.end()) {
                        Chunk &neighborChunk = it->second;

                        if (neighborChunk.getBlock(x, y, _chunkSize - 1) == CubeType::AIR)
                            addFace(pos, chunk.getBlock(x, y, z), CubeFace::FRONT, mesh,
                                    vertexOffset);
                    }
                }

                if (x == _chunkSize - 1) { // right on border
                    uint64_t key = makeHash(chunkX + 1, chunkZ);

                    auto it = _activeChunk.find(key);
                    if (it != _activeChunk.end()) {
                        Chunk &neighborChunk = it->second;

                        if (neighborChunk.getBlock(0, y, z) == CubeType::AIR)
                            addFace(pos, chunk.getBlock(x, y, z), CubeFace::RIGHT, mesh,
                                    vertexOffset);
                    }
                }

                if (z == _chunkSize - 1) { // back on border
                    uint64_t key = makeHash(chunkX, chunkZ + 1);

                    auto it = _activeChunk.find(key);
                    if (it != _activeChunk.end()) {
                        Chunk &neighborChunk = it->second;

                        if (neighborChunk.getBlock(x, y, 0) == CubeType::AIR)
                            addFace(pos, chunk.getBlock(x, y, z), CubeFace::BACK, mesh,
                                    vertexOffset);
                    }
                }
            }
        }
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
    uint64_t neighbor[4] = {makeHash(chunkX - 1, chunkZ), makeHash(chunkX + 1, chunkZ),
                            makeHash(chunkX, chunkZ - 1), makeHash(chunkX, chunkZ + 1)};

    for (auto &key : neighbor) {
        auto it = _activeChunk.find(key);
        if (it != _activeChunk.end())
            it->second.dirty = true;
    }
}
