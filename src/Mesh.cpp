#include "Mesh.hpp"

Mesh::Mesh(const tCHUNK &chunk, const unsigned int &size, const unsigned int drawType) {
    index_count = chunk.indices.size();
    vertex_count = chunk.vertices.size() / 3;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    if (!chunk.indices.empty())
        glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, chunk.vertices.size() * sizeof(float), chunk.vertices.data(),
                 GL_STATIC_DRAW);

    if (!chunk.indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk.indices.size() * sizeof(unsigned int),
                     chunk.indices.data(), drawType);
    }

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glBindVertexArray(0);
}

Mesh &Mesh::operator=(Mesh other) {
    if (this != &other) {
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        index_count = other.index_count;
        vertex_count = other.vertex_count;
    }
    return *this;
}

Mesh::Mesh(Mesh &&other) noexcept
    : m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO), index_count(other.index_count),
      vertex_count(other.vertex_count) {
    // m_VAO = other.m_VAO;
    // m_VBO = other.m_VBO;
    // m_EBO = other.m_EBO;
    // index_count = other.index_count;
    // vertex_count = other.vertex_count;

    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
    other.index_count = 0;
    other.vertex_count = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);

        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;

        other.m_VAO = 0;
        other.m_VBO = 0;
    }
    return *this;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    if (m_EBO)
        glDeleteBuffers(1, &m_EBO);
}

void Mesh::draw() {
    glBindVertexArray(m_VAO);
    if (index_count)
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    glBindVertexArray(0);
}
