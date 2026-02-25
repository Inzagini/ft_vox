#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Camera.hpp"
#include "CameraControl.hpp"
#include "ChunkGenerator.hpp"
#include "ChunkManager.hpp"
#include "CubeData.hpp"
#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include <iostream>

void processInput(GLFWwindow *window);

class Window;
