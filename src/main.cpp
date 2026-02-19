#include "main.hpp"
#include "Mesh.hpp"
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

    // veritces
    glm::vec3 cubeVertices[8] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},  {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f},
    };

    // Indices for the cube (two triangles per face, 36 indices)
    unsigned int cubeIndices[36] = {
        0, 1, 2, 2, 3, 0, // back
        4, 5, 6, 6, 7, 4, // front
        3, 2, 6, 6, 7, 3, // top
        0, 1, 5, 5, 4, 0, // bottom
        1, 2, 6, 6, 5, 1, // right
        0, 3, 7, 7, 4, 0  // left
    };

    const int CHUNK_SIZE = 16;

    while (!glfwWindowShouldClose(window.get())) {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.get());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<float> terrainVertices;
        std::vector<unsigned int> terrainIndices;
        int vertexOffset = 0;

        const float scale = 0.01f;
        const float heightMultiplier = 50.0f;

        PerlinNoise perlin(1234);
        std::vector<std::vector<int>> heightMap(CHUNK_SIZE, std::vector<int>(CHUNK_SIZE));

        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                float height = std::floor(perlin.octaveNoise(x * scale, z * scale, 6, 0.5, 2.0) *
                                          heightMultiplier);
                heightMap[x][z] = height;
                for (int y = 0; y <= height; y++) {
                    glm::vec3 offsetPos(x, y, z);

                    // Add cube vertices
                    for (int i = 0; i < 8; i++) {
                        terrainVertices.push_back(cubeVertices[i].x + offsetPos.x);
                        terrainVertices.push_back(cubeVertices[i].y + offsetPos.y);
                        terrainVertices.push_back(cubeVertices[i].z + offsetPos.z);
                        // Optional: add normals/texcoords here
                    }

                    // Add cube indices
                    for (int i = 0; i < 36; i++) {
                        terrainIndices.push_back(cubeIndices[i] + vertexOffset);
                    }

                    vertexOffset += 8; // 8 unique vertices per cube
                }
            }
        }

        // Create mesh once
        Mesh terrainMesh(terrainVertices, 3, GL_STATIC_DRAW, terrainIndices);

        shader.use();

        {
            glm::mat4 projection = glm::perspective(
                glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            shader.setMat4("projection", projection);

            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            shader.setMat4("view", view);

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
