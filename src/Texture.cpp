#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr void TextureRegistry::setBlockTexture(const uint8_t blockType,
                                                const uint16_t textureIndex) {
    for (auto &face : {CubeFace::TOP, CubeFace::BOTTOM, CubeFace::LEFT, CubeFace::RIGHT,
                       CubeFace::FRONT, CubeFace::BACK})
        blockTextures[blockType][static_cast<uint8_t>(face)] = textureIndex;
}

constexpr void TextureRegistry::setBlockTexture(const uint8_t blockType, const uint16_t top,
                                                const uint16_t bottom, const uint16_t side) {
    blockTextures[blockType][static_cast<uint8_t>(CubeFace::TOP)] = top;
    blockTextures[blockType][static_cast<uint8_t>(CubeFace::BOTTOM)] = bottom;
    for (auto &face : {CubeFace::LEFT, CubeFace::RIGHT, CubeFace::FRONT, CubeFace::BACK})
        blockTextures[blockType][static_cast<uint8_t>(face)] = side;
}

glm::vec2 TextureRegistry::getTextureUV(const CubeType blockType, const CubeFace face) {
    uint16_t textureIndex =
        blockTextures[static_cast<uint8_t>(blockType)][static_cast<uint8_t>(face)];
    return Atlas::getUV(textureIndex);
}

const unsigned int TextureRegistry::getTextureID() { return textureID; }

void TextureRegistry::initialize() {
    loadBitMap();

    setBlockTexture(static_cast<uint8_t>(CubeType::GRASS), Atlas::indexing(13, 10),
                    Atlas::indexing(8, 5), 320);

    setBlockTexture(static_cast<uint8_t>(CubeType::DIRT), Atlas::indexing(4, 8));

    setBlockTexture(static_cast<uint8_t>(CubeType::STONE), Atlas::indexing(10, 19));
}

bool TextureRegistry::loadBitMap() {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("resources/texture/atlas.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture" << std::endl;
        return false;
    }
    stbi_image_free(data);

    return true;
}
