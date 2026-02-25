#pragma once

#include "Camera.hpp"

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
    glm::vec3 getCameraPos() const { return camera.getPos(); }
    float getYaw() const { return camera.getYaw(); }

  private:
    Camera &camera;
    float cameraSpeed{1.0f};
};
