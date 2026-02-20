#pragma once

#include "ChunkGenerator.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>

using keyHash = long long;

class ChunkManager {
  public:
    ChunkManager(const int seed) : seed(seed), generator(seed) {}
    void createChunk(const int chunkX, const int chunkZ);
    void render(Shader &shader);
    void unload(const int chunkX, const int chunkZ);
    void update(const glm::vec3 &pos);

  private:
    keyHash makeKey(int x, int z) {
        return (static_cast<long long>(x) << 32) | (static_cast<long long>(x));
    }
    int extractX(keyHash key) { return static_cast<int>(key >> 32); }

    int extractZ(keyHash key) { return static_cast<int>(key & 0xFFFFFF); }

  private:
    glm::vec3 playerPos;
    int seed;
    int renderDistance{5};
    ChunkGenerator generator;
    std::unordered_map<keyHash, Mesh> activeChunk;

    std::map<std::pair<int, int>, std::unique_ptr<Mesh>> mp;
};
