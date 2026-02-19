#pragma once
#include "glm/glm.hpp"
// veritces
// clang-format off
    inline constexpr glm::vec3 cubevertices[8] = {
        {-0.5f, -0.5f, -0.5f} , {0.5f, -0.5f, -0.5f}, //back bottom 0 1
        {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f}, //back upper 2 3
        {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f}, //front bottom 4 5
        {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f}, //front upper 6 7
    };
// clang-format on

// indices for the cube (two triangles per face, 36 indices)
inline constexpr unsigned int cubeindices[36] = {
    0, 1, 2, 2, 3, 0, // back 0 5
    4, 5, 6, 6, 7, 4, // front 6 11
    3, 2, 6, 6, 7, 3, // top 12 17
    0, 1, 5, 5, 4, 0, // bottom 18 23
    1, 2, 6, 6, 5, 1, // right 24 29
    0, 3, 7, 7, 4, 0  // left 30 35
};

enum class CubeFace { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK };

const glm::vec3 cubeFaceVertices[6][4] = {
    // TOP (y+1)
    {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}},
    // BOTTOM (y)
    {{0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}},
    // LEFT (x)
    {{0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}},
    // RIGHT (x+1)
    {{1, 0, 0}, {1, 0, 1}, {1, 1, 1}, {1, 1, 0}},
    // FRONT (z)
    {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}},
    // BACK (z+1)
    {{0, 0, 1}, {0, 1, 1}, {1, 1, 1}, {1, 0, 1}}};

const unsigned int cubeFaceIndices[6] = {0, 1, 2, 0, 2, 3};
