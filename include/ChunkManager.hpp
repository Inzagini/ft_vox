#pragma once

#include "Camera.hpp"
#include "ChunkGenerator.hpp"
#include "Shader.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <unordered_map>
#include <utility>

class ChunkManager {
  public:
    ChunkManager(const int seed) : seed(seed), generator(seed) {
        activeChunk.reserve(renderDistance * renderDistance);
    }
    void createChunk(const int chunkX, const int chunkZ);
    void render(Shader &shader, const glm::vec3 &playerPos, Camera &camera);
    void unload(const int chunkX, const int chunkZ);
    void update(Camera &camera);
    void addFaces(Chunk &_chunk, const int chunkX, const int chunkZ);
    void meshing(const int chunkX, const int chunkZ, Camera &camera);

  private:
    bool isNeighborBlockAir();
    void markNeigborChunkDirty(const int chunkX, const int chunkZ);
    void addFace(glm::vec3 &pos, int face, tCHUNK &chunk, int &vertexOffset);
    struct pairHash {
        std::size_t operator()(const std::pair<int, int> &p) const noexcept {
            std::size_t h1 = std::hash<int>{}(p.first);
            std::size_t h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    uint64_t makeHash(const int chunkX, const int chunkZ) {
        return (static_cast<uint64_t>(chunkX) << 32 | static_cast<uint32_t>(chunkZ));
    }

    int getXFromHash(uint64_t hash) { return static_cast<int>(hash >> 32); }
    int getZFromHash(uint64_t hash) { return static_cast<int>(hash & 0xFFFFFFFFULL); }

  private:
    const int seed;
    const int renderDistance{10};
    const int chunkSize{16};
    ChunkGenerator generator;
    std::unordered_map<std::pair<int, int>, Chunk, pairHash> activeChunk;

    // TODO: future change in keyhasing a paring bit shifting for cords texture and block type
    std::vector<std::pair<uint64_t, Mesh>> meshedChunk;
};
