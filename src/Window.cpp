#include "Window.hpp"
#include <iostream>

Window::Window()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scop", nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Window failed to create. Terminating\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(this->m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
}

Window::~Window()
{
    if (m_window)
        glfwDestroyWindow(m_window);
}

GLFWwindow* Window::get() const
{
    return m_window;
}



