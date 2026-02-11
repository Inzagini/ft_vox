#include "main.hpp"
#include <fstream>


bool isArch()
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


    while (!glfwWindowShouldClose(window.get()))
    {
        generalInputProcess(window.get());


        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

