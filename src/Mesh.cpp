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
