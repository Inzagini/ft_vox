#pragma once

#include "Window.hpp"

#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
  public:
    glm::mat4 getViewMat() const { return glm::lookAt(position, position + front, up); }
    glm::mat4 getProjection() const {
        return glm::perspective(glm::radians(fov), aspectRatio, near, far);
    }

    glm::vec3 getFront() { return front; }
    void setFront(const glm::vec3 &v) { front = v; }
    glm::vec3 getUp() { return up; }

    void move(const glm::vec3 &v) { position = v; }
    glm::vec3 getPos() const { return position; }

    float getYaw() { return yaw; }
    void setYaw(const float &n) { yaw = n; }

    float getPitch() { return pitch; }
    void setPitch(const float &n) { pitch = n; }

  private:
    glm::vec3 position{0.0f, 1.8f, 3.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float yaw{-90.0f}; // yaw is initialized to -90.0 degrees
                       // since a yaw of 0.0 results in a direction
    float pitch{0.0f};
    float fov{70.f};
    float near{0.1f};
    float far{100.0f};
    float aspectRatio = static_cast<float>(SCR_WIDTH / SCR_HEIGHT);
};
