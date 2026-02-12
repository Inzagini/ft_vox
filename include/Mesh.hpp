#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include <vector>

struct Vertex
{
	glm::vec3 pos;
};

struct Obj
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t _size;
};

class Mesh
{
	public:
		Mesh(const Obj &obj);
		~Mesh();
		void draw();

	private:
		uint32_t m_VAO{}, m_VBO{}, m_EBO{};
};
