#include "CameraControl.hpp"

void CameraControl::handleKeyboardInput(InputAction action) {
    glm::vec3 cameraFront = camera.getFront();
    glm::vec3 flatFront = glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    glm::vec3 cameraPos = camera.getPos();

    // if (action == InputAction::MOVE_FORWARD)
    //     cameraPos += cameraSpeed * flatFront;
    // if (action == InputAction::MOVE_BACKWARD)
    //     cameraPos -= cameraSpeed * flatFront;
    // if (action == InputAction::MOVE_LEFT)
    //     cameraPos -= glm::normalize(glm::cross(cameraFront, camera.getUp())) * cameraSpeed;
    // if (action == InputAction::MOVE_RIGHT)
    //     cameraPos += glm::normalize(glm::cross(cameraFront, camera.getUp())) * cameraSpeed;
    // if (action == InputAction::MOVE_FLY_UP)
    //     cameraPos += cameraSpeed * glm::vec3{0, 1, 0};
    // if (action == InputAction::MOVE_FLY_DOWN)
    //     cameraPos -= cameraSpeed * glm::vec3{0, 1, 0};

    switch (action) {
    case InputAction::MOVE_FORWARD:
        cameraPos += cameraSpeed * flatFront;
        break;
    case InputAction::MOVE_BACKWARD:
        cameraPos -= cameraSpeed * flatFront;
        break;
    case InputAction::MOVE_LEFT:
        cameraPos -= glm::normalize(glm::cross(cameraFront, camera.getUp())) * cameraSpeed;
        break;
    case InputAction::MOVE_RIGHT:
        cameraPos += glm::normalize(glm::cross(cameraFront, camera.getUp())) * cameraSpeed;
        break;
    case InputAction::MOVE_FLY_UP:
        cameraPos += cameraSpeed * glm::vec3{0, 1, 0};
        break;
    case InputAction::MOVE_FLY_DOWN:
        cameraPos -= cameraSpeed * glm::vec3{0, 1, 0};
        break;
    }
    camera.move(cameraPos);
}

void CameraControl::handleMouseMovement(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    float yaw = camera.getYaw();
    float pitch = camera.getPitch();

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    camera.setPitch(pitch);
    camera.setYaw(yaw);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.setFront(glm::normalize(front));
}
