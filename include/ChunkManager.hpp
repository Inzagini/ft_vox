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
#include <bitset>
#include <unordered_map>
#include <utility>

/*
 * Manager of chunks. chunk are save in unordered map
 * keys of table are chunk's coordinates  (uint64_t)
 * | X(26bits) | Z(26bits) | Y(8bits) | unused 4bits|
 * */

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
    void greedyMesh(Chunk &_chunk); // TODO: greedy meshing
    bool isNeighborBlockAir();
    void markNeigborChunkDirty(const int chunkX, const int chunkZ);
    void addFace(const glm::vec3 &pos, const CubeType &type, CubeFace face, tMesh &chunk,
                 int &vertexOffset);

    uint64_t makeHash(const int chunkX, const int chunkZ) {
        return ((static_cast<uint64_t>(chunkX) & 0x03FFFFFF) << 38) |
               ((static_cast<uint64_t>(chunkZ) & 0x03FFFFFF) << 12);
    }

    int32_t unpackBits(uint32_t hash) { return static_cast<int32_t>(hash << 7) >> 7; }

    int getXFromHash(uint64_t hash) { return static_cast<int>((int64_t)hash >> 38); }
    int getZFromHash(uint64_t hash) { return static_cast<int>(((int64_t)hash << 26) >> 38); }

  private:
    ChunkGenerator generator;
    std::unordered_map<uint64_t, Chunk> _activeChunk;
    const int _seed;
    const int _renderDistance{10};
    const int _loadDistance{_renderDistance + 2};
    const int _chunkSize{16};
    std::mutex activeChunkMutex;
    TextureRegistry &texture;
    ThreadPool &threadPool;
};
