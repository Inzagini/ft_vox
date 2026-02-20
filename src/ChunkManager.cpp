#include "ChunkManager.hpp"
#include "glm/ext/matrix_transform.hpp"

void ChunkManager::createChunk(const int playerChunkX, const int playerChunkZ) {

    for (int dx = -renderDistance; dx <= renderDistance; dx++) {
        for (int dz = -renderDistance; dz <= renderDistance; dz++) {
            int chunkX = playerChunkX + dx;
            int chunkZ = playerChunkZ + dz;
            std::pair<int, int> key = {chunkX, chunkZ};

            if (!mp.contains(key)) {
                std::cout << "Generate chunk \n current size:  " << mp.size() << '\n';
                tCHUNK playerChunk = generator.generate(chunkX, chunkZ);
                mp[key] = std::make_unique<Mesh>(playerChunk, 3, GL_STATIC_DRAW);
            }
        }
    }
}

// TODO:: need to unload chunks that are futher than render distance
void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    for (auto it = mp.begin(); it != mp.end();) {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;
        if (abs(chunkX - playerChunkX) > renderDistance ||
            abs(chunkZ - playerChunkZ) > renderDistance) {
            std::cout << "Delete chunk \n current size: " << mp.size() << '\n';
            it = mp.erase(it); // safely remove outside chunk
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

    for (auto &[key, mesh] : mp) {
        glm::mat4 model = glm::mat4(1.0f);

        // Offset the chunk in world space (assuming chunk size = 16)
        model = glm::translate(model, glm::vec3(key.first, 0.0f, key.second));
        shader.setMat4("model", model);
        mesh->draw();
    }
}
