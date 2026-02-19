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
    void handleKeyboardInput(InputAction);
    void handleMouseMovement(float xoffset, float yoffset);

  private:
    Camera &camera;
    float cameraSpeed{0.5f};
};
