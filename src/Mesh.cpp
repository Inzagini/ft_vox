#include "Mesh.hpp"

Mesh::Mesh(const tMesh &chunk, const unsigned int &size, const unsigned int drawType) {
    _indexCount = chunk.indices.size();
    _vertexCount = chunk.vertices.size() / 3;

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    if (!chunk.indices.empty())
        glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, chunk.vertices.size() * sizeof(float), chunk.vertices.data(),
                 GL_STATIC_DRAW);

    if (!chunk.indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk.indices.size() * sizeof(unsigned int),
                     chunk.indices.data(), drawType);
    }

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // face attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glBindVertexArray(0);
}

Mesh &Mesh::operator=(Mesh other) {
    if (this != &other) {
        _VAO = other._VAO;
        _VBO = other._VBO;
        _EBO = other._EBO;
        _indexCount = other._indexCount;
        _vertexCount = other._vertexCount;
    }
    return *this;
}

Mesh::Mesh(Mesh &&other) noexcept
    : _VAO(other._VAO), _VBO(other._VBO), _EBO(other._EBO), _indexCount(other._indexCount),
      _vertexCount(other._vertexCount) {

    other._VAO = 0;
    other._VBO = 0;
    other._EBO = 0;
    other._indexCount = 0;
    other._vertexCount = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);

        _VAO = other._VAO;
        _VBO = other._VBO;
        _EBO = other._EBO;

        other._VAO = 0;
        other._VBO = 0;
        other._EBO = 0;
    }
    return *this;
}

Mesh::~Mesh() {
    if (_VAO)
        glDeleteVertexArrays(1, &_VAO);
    if (_VBO)
        glDeleteBuffers(1, &_VBO);
    if (_EBO)
        glDeleteBuffers(1, &_EBO);
}

void Mesh::draw() {
    glBindVertexArray(_VAO);
    if (_indexCount)
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, _vertexCount);

    glBindVertexArray(0);
}
