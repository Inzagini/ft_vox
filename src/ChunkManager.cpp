#include "ChunkManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <memory>

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -renderDistance; dx <= renderDistance; dx++) {
        for (int dz = -renderDistance; dz <= renderDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> key = {chunkX, chunkZ};

            if (!activeChunk.contains(key)) {
                tCHUNK chunk = generator.generate(chunkX, chunkZ);
                activeChunk[key] = std::make_unique<Mesh>(chunk, 3, GL_STATIC_DRAW);
                // activeChunk.emplace(key, std::make_unique<Mesh>(chunk, 3u, GL_STATIC_DRAW));
            }
        }
    }
}

// TODO:: need to unload chunks that are futher than render distance
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

void ChunkManager::update(const glm::vec3 &pos) {
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    createChunk(chunkX, chunkZ);

    unload(chunkX, chunkZ);
}

void ChunkManager::render(Shader &shader) {

    for (auto &[key, mesh] : activeChunk) {
        glm::mat4 model = glm::mat4(1.0f);

        // Offset the chunk in world space (assuming chunk size = 16)
        model = glm::translate(model, glm::vec3(key.first, 0.0f, key.second));
        shader.setMat4("model", model);
        mesh->draw();
    }
}
