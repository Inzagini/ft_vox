#include "main.hpp"

int main() {
    Window window;
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    TextureRegistry texture;

    window.setup();

    Camera camera;
    CameraControl camControl(camera);
    int seed = 1;

    glfwSetCursorPosCallback(window.get(), camControl.mouseMovementCallback);
    glfwSetWindowUserPointer(window.get(), &camControl);
    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    ThreadPool threadPool(3);
    ChunkManager chunkManager(seed, texture, threadPool);

    while (!glfwWindowShouldClose(window.get())) {

        InputProcess::handler(window.get(), camControl);

        glClearColor(0.431f, 0.494f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("projection", camera.getProjection());
        shader.setMat4("view", camera.getViewMat());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.getTextureID()); // ID from glGenTextures
        shader.setInt("atlas", 0);

        chunkManager.update(camera);
        chunkManager.render(shader, camera.getPos(), camera);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    return 0;
}
