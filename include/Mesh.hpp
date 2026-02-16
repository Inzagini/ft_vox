#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

class Mesh
{
    public:
        Mesh(const std::vector<float> &vertices, const unsigned int &size, const unsigned int drawType,const std::vector<unsigned int> &indices = {});
        ~Mesh();
        void draw();

    private:
        unsigned int    m_VBO{}, m_VAO{}, m_EBO{};
        size_t          indexCount{};
        size_t          vertexCount{};
};
