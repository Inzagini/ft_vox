#include "main.hpp"
#include "PerlinNoise.hpp"

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationSpeed = 1.5f;
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

void processInput(GLFWwindow *window) {
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

    int test;
    float cameraSpeed = static_cast<float>(5.0 * deltaTime);
    glm::vec3 flatFront = glm::normalize(glm::vec3(cameraFront.x, 0, cameraFront.z));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * flatFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * flatFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::vec3{0, 1, 0};
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::vec3{0, 1, 0};
}

int main() {
    Window window;
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window.get(), &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window.get(), mouse_callback);
    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // clang-format off
    //veritces
    const int CHUNK_SIZE = 16;
    const float scale = 0.01f;
    const float heightMultiplier = 50.0f;
    std::vector<float> vertices;
    PerlinNoise perlin(1234);

    for (int z{}; z <= CHUNK_SIZE; z++)
    {
        for (int x{}; x <= CHUNK_SIZE; x++)
        {
            int worldX = CHUNK_SIZE + x;
            int worldZ = CHUNK_SIZE + z;

            float height = perlin.octaveNoise(worldX * scale, worldZ * scale,\
                                  6, 0.5, 2.0);

            height *= heightMultiplier;
            vertices.push_back(worldX);
            vertices.push_back(height);
            vertices.push_back(worldZ);
        }
    }
    // clang-format on

    Mesh mesh1(vertices, 3, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window.get())) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.get());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        {
            glm::mat4 projection = glm::perspective(
                glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            shader.setMat4("projection", projection);

            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            shader.setMat4("view", view);

            int gridSize = 16;
            float spacecing = 1.0f;
            float offset = (gridSize - 1) * spacecing * 0.5f;

            for (unsigned int i{}; i < gridSize; i++) {
                for (unsigned int j{}; j < gridSize; j++) {
                    glm::vec3 newPos(i * spacecing - offset, 0.0f, j * spacecing - offset);
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, newPos);
                    shader.setMat4("model", model);

                    mesh1.draw();
                }
            }
        }

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    return 0;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
