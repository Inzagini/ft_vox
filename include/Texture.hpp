
#pragma once

#include "Atlas.hpp"
#include "CubeData.hpp"
#include "glad/glad.h"
#include <iostream>

class TextureRegistry {
  public:
    TextureRegistry() : blockTextures{} { initialize(); };
    void initialize();
    const unsigned int getTextureID();
    glm::vec2 getTextureUV(const CubeType blockType, const CubeFace face);

  private:
    constexpr void setBlockTexture(const uint8_t blockType, const uint16_t textureIndex);

    constexpr void setBlockTexture(const uint8_t blockType, const uint16_t top,
                                   const uint16_t bottom, const uint16_t side);
    bool loadBitMap();

  private:
    static constexpr int BLOCK_TYPE_COUNT = 6;
    static constexpr int FACE_COUNT = 6;
    unsigned int textureID = 0;
    uint16_t blockTextures[BLOCK_TYPE_COUNT][FACE_COUNT];
};
