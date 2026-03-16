#pragma once

#include "CameraControl.hpp"
#include "Window.hpp"
#include <iostream>

namespace InputProcess {

inline bool wireFrameMode = false;
inline bool zKeyPressed = false;

inline void handler(GLFWwindow *window, CameraControl &cameraControler) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !zKeyPressed) {
        wireFrameMode = !wireFrameMode;
        zKeyPressed = true;

        if (wireFrameMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
        zKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        glm::vec3 pos = cameraControler.getCameraPos();
        std::cout << "Current position x: " << pos.x << " | y: " << pos.y << " | z: " << pos.z
                  << '\n';
    }
    cameraControler.processKeyBoard(window);
}
} // namespace InputProcess
