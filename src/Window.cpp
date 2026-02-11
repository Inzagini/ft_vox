#include "Window.hpp"
#include <iostream>

Window::Window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scop", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Window failed to create. Terminating\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

Window::~Window()
{
    if (window)
        glfwDestroyWindow(window);
}

GLFWwindow* Window::get() const
{
    return window;
}



