#pragma once

#include "ChunkGenerator.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include <functional>
#include <iostream>
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
    struct pairHash {
        std::size_t operator()(const std::pair<int, int> &p) const noexcept {
            std::size_t h1 = std::hash<int>{}(p.first);
            std::size_t h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    int64_t makeKey(int x, int z) { return (int64_t(x) << 32) | uint32_t(z); }

  private:
    glm::vec3 playerPos;
    int seed;
    int renderDistance{5};
    ChunkGenerator generator;
    std::unordered_map<std::pair<int, int>, std::unique_ptr<Mesh>, pairHash> activeChunk;
    std::unordered_map<int64_t, Mesh> testChunk;
    // std::map<std::pair<int, int>, std::unique_ptr<Mesh>> activeChunk;
};
