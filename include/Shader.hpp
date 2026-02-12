#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader
{
	public:
		Shader(const std::string &vertexShaderSourcePath, const std::string &fragmentShaderSourcePath);

	private:
		void checkCompileErrors(unsigned int shader, std::string type);
		uint32_t createAndCompileShader(const unsigned int type, const char* source);
		void attachAndLinkShaders(const uint32_t vertexShader, const uint32_t fragmentShader);
		std::string loadShader(const std::string &name);

		void setVec4(const std::string &name, float x, float y, float z, float w) const
		{glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);}

		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);}

		void setVec3(const std::string &name, const glm::vec3 &value) const
		{glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);}

		void setFloat(const std::string &name, const float n) const
		{glUniform1f(glGetUniformLocation(m_ID, name.c_str()), n);}

		void setBool(const std::string &name, const bool n) const
		{glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int32_t>(n));}

	private:
		uint32_t m_ID;
};
