#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

enum class CubeFace : uint8_t { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK };
enum class CubeType : uint8_t { AIR = 0, GRASS, DIRT, STONE };

constexpr glm::vec3 cubeFaceVertices[6][4] = {
    // TOP (y+1)
    {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}},
    // BOTTOM (y)
    {{0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}},
    // LEFT (x)
    {{0, 0, 1}, {0, 0, 0}, {0, 1, 0}, {0, 1, 1}},
    // RIGHT (x+1)
    {{1, 0, 0}, {1, 0, 1}, {1, 1, 1}, {1, 1, 0}},
    // FRONT (z)
    {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}},
    // BACK (z+1)
    {{1, 0, 1}, {0, 0, 1}, {0, 1, 1}, {1, 1, 1}}};

constexpr unsigned int cubeFaceIndices[6] = {
    0, 1, 2, // first triangle
    0, 2, 3  // second triangle
};
