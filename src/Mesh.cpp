#include "Mesh.hpp"

Mesh::Mesh(const std::vector<float> &vertices, const unsigned int &size, const unsigned int drawType, const std::vector<unsigned int> &indices)
{
    index_count = indices.size();
    vertex_count = vertices.size() / 3;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    if (!indices.empty()) glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    if (!indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), drawType);
    }


    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, size * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glBindVertexArray(0);

}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void Mesh::draw()
{
    glBindVertexArray(m_VAO);
    if (index_count)
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    glBindVertexArray(0);
}
