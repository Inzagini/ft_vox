#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include <GL/gl.h>
#include <vector>

void processInput(GLFWwindow *window);

class Window;
