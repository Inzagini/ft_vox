#include "main.hpp"
#include "Camera.hpp"
#include "CameraControl.hpp"
#include "ChunkGenerator.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// camera
glm::vec3 cameraPos(0.0f, 1.8f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
bool wireFrameMode = false;
bool zKeyPressed = false;
float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction
                    // vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 70.0f;

void processInput(GLFWwindow *window, CameraControl &cameraControler) {
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

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetWindowUserPointer(window, &cameraControler); // pass camera Controler to the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_FLY_UP);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraControler.handleKeyboardInput(InputAction::MOVE_FLY_DOWN);
}

int main() {
    Window window;
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window.get(), &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glEnable(GL_DEPTH_TEST);

    ChunkGenerator generator(1234);
    Camera camera;
    CameraControl camControl(camera);

    while (!glfwWindowShouldClose(window.get())) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.get(), camControl);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tCHUNK chunk = generator.generate();

        // Create mesh once
        Mesh terrainMesh(chunk, 3, GL_STATIC_DRAW);

        shader.use();

        {
            shader.setMat4("projection", camera.getProjection());
            shader.setMat4("view", camera.getViewMat());

            glm::mat4 model = glm::mat4(1.0f);
            shader.setMat4("model", model);

            terrainMesh.draw();
        }

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    return 0;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    static CameraControl *controler =
        reinterpret_cast<CameraControl *>(glfwGetWindowUserPointer(window));
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (controler)
        controler->handleMouseMovement(xoffset, yoffset);
}
