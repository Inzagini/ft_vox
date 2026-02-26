#pragma once

#include "Window.hpp"

#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
  public:
    glm::mat4 getViewMat() const {
        const int chunkSize = 16;
        const int playerChunkX = floor(position.x / chunkSize);
        const int playerChunkZ = floor(position.z / chunkSize);

        const glm::dvec3 chunkOrigin(playerChunkX * chunkSize, 0.0f, playerChunkZ * chunkSize);
        const glm::vec3 relativeCamPos = position - chunkOrigin;

        glm::mat4 view = glm::lookAt(relativeCamPos, relativeCamPos + front, up);
        return view;
    }

    glm::mat4 getProjection() const {
        return glm::perspective(glm::radians(fov), aspectRatio, near, far);
    }

    glm::vec3 getFront() const { return front; }
    void setFront(const glm::vec3 &v) { front = v; }
    glm::vec3 getUp() const { return up; }

    void move(const glm::vec3 &v) { position = v; }
    glm::vec3 getPos() const { return position; }

    float getYaw() const { return yaw; }
    void setYaw(const float &n) { yaw = n; }

    float getPitch() const { return pitch; }
    void setPitch(const float &n) { pitch = n; }

    float getFOV() const { return fov; }

    bool isInFOV(const int chunkX, const int chunkZ) {
        const int chunkSize = 16;
        const int playerChunkX = floor(position.x / 16);
        const int playerChunkZ = floor(position.z / 16);
        const float halfFOV = fov / 2;
        glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0, front.z));

        const int relativeChunkX = chunkX - playerChunkX;
        const int relativeChunkZ = chunkZ - playerChunkZ;

        const glm::vec3 chunkPos =
            glm::vec3(relativeChunkX * chunkSize, 0.0f, relativeChunkZ * chunkSize);

        if (chunkPos == glm::vec3(0.0f))
            return true;

        const glm::vec3 chunkNorm = glm::normalize(chunkPos);
        const float dot = glm::dot(flatFront, chunkNorm);

        return dot > cos(halfFOV);
    }

  private:
    glm::dvec3 position{0.0f, 50.8f, 0.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float yaw{-90.0f}; // yaw is initialized to -90.0 degrees
                       // since a yaw of 0.0 results in a direction
    float pitch{0.0f};
    float fov{70.f};
    float near{0.5f};
    float far{400.0f};
    float aspectRatio = static_cast<float>(SCR_WIDTH / SCR_HEIGHT);
};
