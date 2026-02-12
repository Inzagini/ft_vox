#include "main.hpp"

static bool isArch()
{
    std::ifstream file("/etc/os-release");

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.find("ID=arch") != std::string::npos)
            return true;
    }

    return false;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        std::exit(-1);
    }

    Window window;

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cerr << "FAILED TO START GLAD\n";
        std::exit(-1);
    }

    Shader shader ("shaders/vertex.glsl", "shaders/fragment.glsl");

    float vertices[] = {
    // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };



    while (!glfwWindowShouldClose(window.get()))
    {
        generalInputProcess(window.get());


        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

