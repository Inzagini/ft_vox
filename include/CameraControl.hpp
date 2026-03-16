#pragma once

#include "Camera.hpp"
#include <GLFW/glfw3.h>

enum class InputAction {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_FLY_UP,
    MOVE_FLY_DOWN
};

class CameraControl {
  public:
    CameraControl(Camera &cam) : camera(cam) {}
    void processKeyBoard(GLFWwindow *window);
    void handleKeyboardInput(InputAction);
    void handleMouseMovement(float xoffset, float yoffset);
    glm::vec3 getCameraPos() const { return camera.getPos(); }
    float getYaw() const { return camera.getYaw(); }
    static void mouseMovementCallback(GLFWwindow *window, double xposIn, double yposIn) {
        CameraControl *cameraControl =
            static_cast<CameraControl *>(glfwGetWindowUserPointer(window));

        if (cameraControl)
            cameraControl->onMovement(xposIn, yposIn);
    }

  private:
    void onMovement(double xposIn, double yposIn);

  private:
    Camera &camera;
    float cameraSpeed{2.0f};
    float lastX{0.0f};
    float lastY{0.0f};
    bool firstMouse{true};
};
