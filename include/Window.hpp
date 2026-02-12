#pragma once

#include <GLFW/glfw3.h>

static constexpr int SCR_WIDTH{800};
static constexpr int SCR_HEIGHT{600};

class Window
{
    public:
        Window();
        ~Window();
        GLFWwindow* get() const;
        void control() const
        {
            if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(m_window, true);
        }

    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
        {
            window = window;
            glViewport(0, 0, width, height);
        }

    private:
        GLFWwindow  *m_window;
};
