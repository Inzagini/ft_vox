#pragma once

#include "Atlas.hpp"
#include "Camera.hpp"
#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "ThreadPool.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <unordered_map>
#include <utility>

class ChunkManager {
  public:
    ChunkManager(const int seed, TextureRegistry &texture, ThreadPool &threadPool)
        : _seed(seed), generator(seed), texture(texture), threadPool(threadPool) {
        _activeChunk.reserve(_loadDistance * _loadDistance * 4);
    }
    void render(Shader &shader, const glm::vec3 &playerPos, Camera &camera);
    void update(Camera &camera);

  private:
    void createChunk(const int chunkX, const int chunkZ);
    void addFaces(Chunk &_chunk, const int chunkX, const int chunkZ, tMesh &mesh);
    void unload(const int chunkX, const int chunkZ);
    void meshing(const int chunkX, const int chunkZ, Camera &camera);
    void greedyMesh(Chunk &_chunk);
    bool isNeighborBlockAir();
    void markNeigborChunkDirty(const int chunkX, const int chunkZ);
    void addFace(const glm::vec3 &pos, const CubeType &type, CubeFace face, tMesh &chunk,
                 int &vertexOffset);
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
    ChunkGenerator generator;
    // std::unordered_map<std::pair<int, int>, Chunk, pairHash> _activeChunk;
    std::unordered_map<uint64_t, Chunk> _activeChunk;
    const int _seed;
    const int _renderDistance{10};
    const int _loadDistance{_renderDistance + 2};
    const int _chunkSize{16};
    std::mutex activeChunkMutex;
    TextureRegistry &texture;
    ThreadPool &threadPool;

    // TODO: future change in keyhasing a paring bit shifting for cords texture and block type
    std::vector<std::pair<uint64_t, Mesh>> meshedChunk;
};
