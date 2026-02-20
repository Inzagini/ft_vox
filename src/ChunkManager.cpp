#include "ChunkManager.hpp"
#include "glm/ext/matrix_transform.hpp"

void ChunkManager::createChunk(const int chunkX, const int chunkZ) {
    // long long key = makeKey(chunkX, chunkZ);
    //
    // if (activeChunk.find(key) == activeChunk.end()) {
    //     std::cout << "Generate chunk...\n";
    //     tCHUNK chunk = generator.generate(chunkX, chunkZ);
    //     Mesh chunkMesh(chunk, 3, GL_STATIC_DRAW);
    //
    //     activeChunk.emplace(key, chunkMesh);
    // }

    // std::pair<int, int> key = {chunkX, chunkZ};
    // if (!mp.contains(key)) {
    //     std::cout << "Generate chunk...\n";
    //     tCHUNK chunk = generator.generate(chunkX, chunkZ);
    //     // Mesh chunkMesh{chunk, 3, GL_STATIC_DRAW};
    //     mp.emplace(key, Mesh{chunk, 3, GL_STATIC_DRAW});
    //     std::cout << "Current size: " << mp.size() << '\n';
    // }
}

// TODO:: need to unload chunks that are futher than render distance
void ChunkManager::unload(const int playerChunkX, const int playerChunkZ) {
    int chunkX = abs(playerChunkX - renderDistance);
    int chunkZ = abs(playerChunkZ - renderDistance);

    keyHash key = makeKey(chunkX, chunkZ);

    activeChunk.erase(key);
}

void ChunkManager::update(const glm::vec3 &pos, Shader &shader) {
    int chunkX = floor(pos.x / 16);
    int chunkZ = floor(pos.z / 16);

    // createChunk(chunkX, chunkZ);
    std::pair<int, int> key = {chunkX, chunkZ};
    if (!mp.contains(key)) {
        std::cout << "Generate chunk...\n";
        // Mesh chunkMesh{chunk, 3, GL_STATIC_DRAW};
        mp[key] = generator.generate(chunkX, chunkZ);
        std::cout << "Current size: " << mp.size() << '\n';
    }

    // for (auto &[key, mesh] : mp) {
    //
    //     // Offset the chunk in world space (assuming chunk size = 16)
    //     glm::mat4 model = glm::translate(glm::mat4{1.0f}, glm::vec3(key.first, 0.0f,
    //     key.second)); shader.setMat4("model", model); mesh.draw();
    // }
    for (auto &[key, chunk] : mp) {
        glm::mat4 model = glm::mat4(1.0f);

        // Offset the chunk in world space (assuming chunk size = 16)
        model = glm::translate(model, glm::vec3(key.first, 0.0f, key.second));
        shader.setMat4("model", model);

        Mesh terrainMesh(chunk, 3, GL_STATIC_DRAW);
        terrainMesh.draw();
    }
}

void ChunkManager::render(Shader &shader) {
    // for (auto &[key, mesh] : activeChunk) {
    //     float x = extractX(key);
    //     float z = extractZ(key);
    //     a require"cmp.utils.feedkeys".run(1343)
    //     glm::mat4 model = glm::translate(glm::mat4{1.0f}, glm::vec3(x, 0.0f, z));
    //     shader.setMat4("model", model);
    //     mesh.draw();
    // }

    // for (auto &[key, mesh] : mp) {
    //     glm::mat4 model = glm::mat4(1.0f);
    //
    //     // Offset the chunk in world space (assuming chunk size = 16)
    //     model = glm::translate(model, glm::vec3(key.first, 0.0f, key.second));
    //     shader.setMat4("model", model);
    //     mesh.draw();
    // }
}
