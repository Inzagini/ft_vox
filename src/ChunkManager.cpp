#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "CubeData.hpp"
#include "Texture.hpp"
#include <array>

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
    generatedChunks.reserve((_loadDistance + 1) * (_loadDistance + 1));

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
                it = _activeChunk.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ, Camera &camera) {
    {
        tMesh tmesh;
        for (auto &[key, chunk] : _activeChunk) {
            int chunkX = getXFromHash(key);
            int chunkZ = getZFromHash(key);

            if (!camera.isInFOV(chunkX, chunkZ))
                continue;

            for (int chunkY = 0; chunkY < 24; chunkY++) {

                ChunkSection &section = chunk.sections[chunkY];

                if (section.empty)
                    continue;

                if (section.dirty) {
                    addFaces(chunk, chunkX, chunkZ, chunkY, tmesh);
                    section.mesh.make(tmesh, 5);
                    section.hasMesh = true;
                    section.dirty = false;
                    tmesh.reset();
                }
            }
        }
    }
}

void ChunkManager::render(Shader &shader, const glm::vec3 &playerPos, Camera &camera) {

    const int playerChunkX = floor(playerPos.x / 16);
    const int playerChunkZ = floor(playerPos.z / 16);

    for (auto &[key, chunk] : _activeChunk) {

        const int chunkX = getXFromHash(key);
        const int chunkZ = getZFromHash(key);

        const int relativeChunkX = chunkX - playerChunkX;
        const int relativeChunkZ = chunkZ - playerChunkZ;

        if (!(abs(relativeChunkX) <= _renderDistance && abs(relativeChunkZ) <= _renderDistance &&
              camera.isInFOV(chunkX, chunkZ)))
            continue;

        for (int i = 0; i < 24; i++) {

            auto &sec = chunk.sections[i];
            float yPos = (float)i * 16 - 64;

            if (sec.empty)
                continue;

            const glm::vec3 chunkPos =
                glm::vec3(relativeChunkX * _chunkSize, yPos, relativeChunkZ * _chunkSize);

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPos);
                shader.setMat4("model", model);
                sec.mesh.draw();
            }
        }
    }
}
void ChunkManager::addFaces(Chunk &chunk, const int chunkX, const int chunkZ, const int chunkY,
                            tMesh &mesh) {

    int vertexOffset{};
    auto neighborLeft = _activeChunk.find(makeHash(chunkX - 1, chunkZ));
    auto neighborRight = _activeChunk.find(makeHash(chunkX + 1, chunkZ));
    auto neighborFront = _activeChunk.find(makeHash(chunkX, chunkZ - 1));
    auto neighborBack = _activeChunk.find(makeHash(chunkX, chunkZ + 1));

    for (int z = 0; z < _chunkSize; z++) {
        for (int x = 0; x < _chunkSize; x++) {
            for (int y = 0; y < _chunkSize; y++) {

                ChunkSection &chunkSection = chunk.sections[chunkY];
                CubeType &currentBlock = chunkSection.getBlock(x, y, z);

                if (currentBlock == CubeType::AIR)
                    continue;

                glm::vec3 pos(x, y, z);

                if (y != _chunkSize - 1 &&
                    chunkSection.getBlock(x, y + 1, z) == CubeType::AIR) // top
                    addFace(pos, currentBlock, CubeFace::TOP, mesh, vertexOffset);
                else if (y == 15 && chunkY + 1 < 24 &&
                         chunk.sections[chunkY + 1].getBlock(x, 0, z) == CubeType::AIR)
                    addFace(pos, currentBlock, CubeFace::TOP, mesh, vertexOffset);

                if (y != 0 && chunkSection.getBlock(x, y - 1, z) == CubeType::AIR) // bottom
                    addFace(pos, currentBlock, CubeFace::BOTTOM, mesh, vertexOffset);
                else if (y == 0 && chunkY - 1 >= 0 &&
                         chunk.sections[chunkY - 1].getBlock(x, 15, z) == CubeType::AIR)
                    addFace(pos, currentBlock, CubeFace::BOTTOM, mesh, vertexOffset);

                if (x > 0 && chunkSection.getBlock(x - 1, y, z) == CubeType::AIR) // left
                    addFace(pos, currentBlock, CubeFace::LEFT, mesh, vertexOffset);
                else if (x == 0 && neighborLeft != _activeChunk.end()) { // left on border
                    ChunkSection &neighborChunk = neighborLeft->second.sections[chunkY];

                    if (neighborChunk.getBlock(_chunkSize - 1, y, z) == CubeType::AIR)
                        addFace(pos, currentBlock, CubeFace::LEFT, mesh, vertexOffset);
                }

                if (x < _chunkSize - 1 &&
                    chunkSection.getBlock(x + 1, y, z) == CubeType::AIR) // right
                    addFace(pos, currentBlock, CubeFace::RIGHT, mesh, vertexOffset);
                else if (x == _chunkSize - 1 &&
                         neighborRight != _activeChunk.end()) { // right on border
                    ChunkSection &neighborChunk = neighborRight->second.sections[chunkY];

                    if (neighborChunk.getBlock(0, y, z) == CubeType::AIR)
                        addFace(pos, currentBlock, CubeFace::RIGHT, mesh, vertexOffset);
                }

                if (z > 0 && chunkSection.getBlock(x, y, z - 1) == CubeType::AIR) // front
                    addFace(pos, currentBlock, CubeFace::FRONT, mesh, vertexOffset);
                else if (z == 0 && neighborFront != _activeChunk.end()) { // front on border
                    ChunkSection &neighborChunk = neighborFront->second.sections[chunkY];

                    if (neighborChunk.getBlock(x, y, _chunkSize - 1) == CubeType::AIR)
                        addFace(pos, currentBlock, CubeFace::FRONT, mesh, vertexOffset);
                }

                if (z < _chunkSize - 1 &&
                    chunkSection.getBlock(x, y, z + 1) == CubeType::AIR) // back
                    addFace(pos, currentBlock, CubeFace::BACK, mesh, vertexOffset);
                else if (z == _chunkSize - 1 &&
                         neighborBack != _activeChunk.end()) { // back on border
                    ChunkSection &neighborChunk = neighborBack->second.sections[chunkY];

                    if (neighborChunk.getBlock(x, y, 0) == CubeType::AIR)
                        addFace(pos, currentBlock, CubeFace::BACK, mesh, vertexOffset);
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

    const std::array neightbor = {_activeChunk.find(makeHash(chunkX - 1, chunkZ)),
                                  _activeChunk.find(makeHash(chunkX + 1, chunkZ)),
                                  _activeChunk.find(makeHash(chunkX, chunkZ - 1)),
                                  _activeChunk.find(makeHash(chunkX, chunkZ + 1))};

    auto it = _activeChunk.find(makeHash(chunkX, chunkZ));

    if (it != _activeChunk.end()) {
        ChunkSection(&chunkSection)[24] = it->second.sections;

        for (int i = 0; i < 24; i++) {

            if (!chunkSection[i].dirty)
                continue;

            for (auto &it : neightbor) {
                if (it != _activeChunk.end())
                    it->second.sections[i].dirty = true;
            }
        }
    }
}
