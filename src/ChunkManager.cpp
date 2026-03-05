#include "ChunkManager.hpp"
#include "Atlas.hpp"
#include "CubeData.hpp"
#include "Texture.hpp"

void ChunkManager::update(Camera &camera) {
    glm::vec3 pos = camera.getPos();
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);
    meshing(chunkX, chunkZ, camera);
    unload(chunkX, chunkZ);
}

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -_loadDistance; dx <= _loadDistance; dx++) {
        for (int dz = -_loadDistance; dz <= _loadDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> key = {chunkX, chunkZ};

            if (_activeChunk.contains(key))
                continue;

            Chunk chunk;
            generator.generateChunk(chunk, chunkX, chunkZ);

            if (!_activeChunk.contains({chunkX - 1, chunkZ}) ||
                !_activeChunk.contains({chunkX + 1, chunkZ}) ||
                !_activeChunk.contains({chunkX, chunkZ - 1}) ||
                !_activeChunk.contains({chunkX, chunkZ + 1}))
                chunk.dirty = true;

            _activeChunk.try_emplace(key, std::move(chunk));
            markNeigborChunkDirty(chunkX, chunkZ);
        }
    }
}

void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    for (auto it = _activeChunk.begin(); it != _activeChunk.end();) {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;
        if (abs(chunkX - playerChunkX) > _loadDistance ||
            abs(chunkZ - playerChunkZ) > _loadDistance) {
            it = _activeChunk.erase(it); // safely remove outside chunk
        } else {
            ++it;
        }
    }
}

void ChunkManager::meshing(const int chunkX, const int chunkZ, Camera &camera) {

    for (auto &[key, chunk] : _activeChunk) {
        if ((chunk.mesh == nullptr || chunk.dirty) && camera.isInFOV(key.first, key.second)) {
            tMesh mesh;
            addFaces(chunk, key.first, key.second, mesh);
            chunk.mesh = std::make_unique<Mesh>(mesh, 5, GL_STATIC_DRAW);
            chunk.dirty = false;
        }
    }
}

void ChunkManager::render(Shader &shader, const glm::vec3 &playerPos, Camera &camera) {

    const int playerChunkX = floor(playerPos.x / 16);
    const int playerChunkZ = floor(playerPos.z / 16);

    for (auto &[key, chunk] : _activeChunk) {

        const int relativeChunkX = key.first - playerChunkX;
        const int relativeChunkZ = key.second - playerChunkZ;
        const glm::vec3 chunkPos =
            glm::vec3(relativeChunkX * _chunkSize, 0.0f, relativeChunkZ * _chunkSize);

        if (abs(relativeChunkX) <= _renderDistance && abs(relativeChunkZ) <= _renderDistance &&
            camera.isInFOV(key.first, key.second)) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), chunkPos);
            shader.setMat4("model", model);
            chunk.mesh->draw();
        }
    }
}
void ChunkManager::addFaces(Chunk &chunk, const int chunkX, const int chunkZ, tMesh &mesh) {

    int vertexOffset{};
    for (int z = 0; z < _chunkSize; z++) {
        for (int x = 0; x < _chunkSize; x++) {
            for (int y = 0; y < 256; y++) {
                glm::vec3 pos(x, y, z);

                if (chunk.block[x][z][y] == CubeType::AIR)
                    continue;

                if (chunk.block[x][z][y + 1] == CubeType::AIR) // top
                    addFace(pos, chunk.block[x][z][y], CubeFace::TOP, mesh, vertexOffset);

                if (y != 0 && chunk.block[x][z][y - 1] == CubeType::AIR) // bottom
                    addFace(pos, chunk.block[x][z][y], CubeFace::BOTTOM, mesh, vertexOffset);

                if (x > 0 && chunk.block[x - 1][z][y] == CubeType::AIR) // left
                    addFace(pos, chunk.block[x][z][y], CubeFace::LEFT, mesh, vertexOffset);

                if (x < _chunkSize - 1 && chunk.block[x + 1][z][y] == CubeType::AIR) // right
                    addFace(pos, chunk.block[x][z][y], CubeFace::RIGHT, mesh, vertexOffset);

                if (z > 0 && chunk.block[x][z - 1][y] == CubeType::AIR) // front
                    addFace(pos, chunk.block[x][z][y], CubeFace::FRONT, mesh, vertexOffset);

                if (z < _chunkSize - 1 && chunk.block[x][z + 1][y] == CubeType::AIR) // back
                    addFace(pos, chunk.block[x][z][y], CubeFace::BACK, mesh, vertexOffset);

                if (x == 0) { // left on border
                    std::pair<int, int> key{chunkX - 1, chunkZ};

                    if (_activeChunk.contains(key)) {
                        Chunk &neighborChunk = _activeChunk[key];

                        if (neighborChunk.block[15][z][y] == CubeType::AIR)
                            addFace(pos, chunk.block[x][z][y], CubeFace::LEFT, mesh, vertexOffset);
                    }
                }

                if (x == _chunkSize - 1) { // right on border
                    std::pair<int, int> key{chunkX + 1, chunkZ};

                    if (_activeChunk.contains(key)) {
                        Chunk &neighborChunk = _activeChunk[key];

                        if (neighborChunk.block[0][z][y] == CubeType::AIR)
                            addFace(pos, chunk.block[x][z][y], CubeFace::RIGHT, mesh, vertexOffset);
                    }
                }

                if (z == 0) { // front on border
                    std::pair<int, int> key{chunkX, chunkZ - 1};

                    if (_activeChunk.contains(key)) {
                        Chunk &neighborChunk = _activeChunk[key];

                        if (neighborChunk.block[x][15][y] == CubeType::AIR)
                            addFace(pos, chunk.block[x][z][y], CubeFace::FRONT, mesh, vertexOffset);
                    }
                }

                if (z == _chunkSize - 1) { // back on border
                    std::pair<int, int> key{chunkX, chunkZ + 1};

                    if (_activeChunk.contains(key)) {
                        Chunk &neighborChunk = _activeChunk[key];

                        if (neighborChunk.block[x][0][y] == CubeType::AIR)
                            addFace(pos, chunk.block[x][z][y], CubeFace::BACK, mesh, vertexOffset);
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
    std::pair<int, int> neighbor[4] = {
        {chunkX - 1, chunkZ}, {chunkX + 1, chunkZ}, {chunkX, chunkZ - 1}, {chunkX, chunkZ + 1}};

    for (auto &key : neighbor) {
        if (_activeChunk.contains(key))
            _activeChunk[key].dirty = true;
    }
}
