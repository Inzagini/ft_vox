#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <ostream>
#include <string>

enum class CubeFace : uint8_t { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK };

// INFO: when size of the CubeType is change need to update number of Types in Texture.hpp
enum class CubeType : uint8_t { AIR = 0, GRASS, DIRT, STONE, WATER, BEDROCK };

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

inline std::ostream &operator<<(std::ostream &o, CubeType type) {
    std::string sType;

    if (type == CubeType::AIR)
        sType = "AIR";
    else if (type == CubeType::GRASS)
        sType = "GRASS";
    else if (type == CubeType::DIRT)
        sType = "DIRT";
    else if (type == CubeType::STONE)
        sType = "STONE";
    else if (type == CubeType::BEDROCK)
        sType = "BEDROCK";

    return o << sType;
}
