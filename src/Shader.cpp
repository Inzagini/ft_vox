#include "Shader.hpp"

Shader::Shader(const std::string &vertexShaderSourcePath, const std::string &fragmentShaderSourcePath)
{
	std::string vertexShaderSource = loadShader(vertexShaderSourcePath);
	std::string fragmentShaderSource = loadShader(fragmentShaderSourcePath);

	uint32_t vertexShader = createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
	uint32_t fragmentShader = createAndCompileShader(GL_VERTEX_SHADER, fragmentShaderSource.c_str());
	attachAndLinkShaders(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

uint32_t Shader::createAndCompileShader(const unsigned int type, const char* source)
{
	uint32_t shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	checkCompileErrors(shader, "COMPILE");
	return shader;
}

std::string Shader::loadShader(const std::string &filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cerr << "FILE: " << filename << " CANNOT BE OPEN\n";
		std::exit(-1);
	}

	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	return ss.str();
}

void Shader::attachAndLinkShaders(const uint32_t vertexShader, const uint32_t fragmentShader)
{
	float ID = glCreateProgram();
	glAttachShader(m_ID, vertexShader);
	glAttachShader(m_ID, fragmentShader);
	glLinkProgram(m_ID);
	checkCompileErrors(m_ID, "LINK");
	m_ID = static_cast<uint32_t>(ID);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "COMPILE")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
					 << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
					 << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
