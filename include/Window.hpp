#pragma once

#include <GLFW/glfw3.h>

static constexpr float SCR_WIDTH{1920};
static constexpr float SCR_HEIGHT{1080};

class Window {
  public:
    Window();
    ~Window();
    GLFWwindow *get() const;

  private:
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

  private:
    GLFWwindow *window;
};
