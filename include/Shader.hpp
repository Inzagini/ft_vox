#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {

    public:
        Shader(const std::string &vertexShaderSourcePath, const std::string &fragmentShaderSourcePath);
        void use(void);
        unsigned int getID();
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w) const;
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;


    private:
        void ErrorMessage(unsigned int shader, unsigned int type);
        unsigned int createAndCompileShader(unsigned int type, const char* source);
        void attachAndLinkShaders(unsigned int vertexShader, unsigned int fragmentShader);
        std::string loadShader(const std::string &name);

    private:
        unsigned int m_ID;

};
