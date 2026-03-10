#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

struct tMesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    tMesh() {
        vertices.reserve(600);
        indices.reserve(600);
    }

    void reset() {
        vertices.clear();
        indices.clear();
    }
};

class Mesh {
  public:
    Mesh() = default;
    Mesh &operator=(Mesh);
    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;
    ~Mesh();
    void draw();
    void make(const tMesh &chunk, const unsigned int &size);

  private:
    unsigned int _VBO{}, _VAO{}, _EBO{};
    size_t _indexCount{};
    size_t _vertexCount{};
};
