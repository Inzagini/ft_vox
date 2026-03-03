#pragma once

#include "glm/glm.hpp"
#include <cstdint>

namespace Atlas {
constexpr int ATLAS_WITDH = 512;
constexpr int ATLAS_HEIGHT = 256;
constexpr int TILE_SIZE = 16;

constexpr int TILES_X = ATLAS_WITDH / TILE_SIZE;
constexpr int TILES_Y = ATLAS_HEIGHT / TILE_SIZE;

inline glm::vec2 getUV(uint16_t index) {
    int x = index % TILES_X;
    int y = index / TILES_X;

    y = TILES_Y - 1 - y;

    float u = x * (float)TILE_SIZE / ATLAS_WITDH;
    float v = y * (float)TILE_SIZE / ATLAS_HEIGHT;

    return glm::vec2(u, v);
}
inline uint8_t indexing(int x, int y) { return x * TILES_X + y; }

constexpr glm::vec2 getTileSize() {
    return glm::vec2((float)TILE_SIZE / ATLAS_WITDH, (float)TILE_SIZE / ATLAS_HEIGHT);
}

} // namespace Atlas
