#pragma once

#include <GLFW/glfw3.h>

static constexpr int SCR_WIDTH{1920};
static constexpr int SCR_HEIGHT{1080};

class Window
{
    public:
        Window();
        ~Window();
        GLFWwindow* get() const;
    
    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }
    
    private:
        GLFWwindow  *window;
};
