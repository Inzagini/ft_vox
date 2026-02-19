#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

struct tCHUNK {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class Mesh {
  public:
    Mesh(const tCHUNK &chunk, const unsigned int &size, const unsigned int drawType);
    ~Mesh();
    void draw();

  private:
    unsigned int m_VBO{}, m_VAO{}, m_EBO{};
    size_t index_count{};
    size_t vertex_count{};
};
