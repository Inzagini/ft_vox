#pragma once

#include "Mesh.hpp"
#include "Shader.hpp"
#include "UIElement.hpp"
#include <vector>

class CrossHair : public UIElement {

  public:
    CrossHair() { setup(); }

    void draw() override {
        glDisable(GL_DEPTH_TEST);
        // shader
        glBindVertexArray(_VAO);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }

  private:
    unsigned _VAO, _VBO;
    void setup() {

        std::vector<float> vertices = {-0.02f, 0.0f,   0.0f, 0.02f, 0.0f,  0.0f,
                                       0.0f,   -0.02f, 0.0f, 0.0f,  0.02f, 0.0f};
        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glBindVertexArray(_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
};
