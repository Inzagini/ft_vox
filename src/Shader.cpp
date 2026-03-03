#include "Shader.hpp"

Shader::Shader(const std::string &vertexShaderSourcePath,
               const std::string &fragmentShaderSourcePath) {
    std::string vertexShaderSource = loadShader(vertexShaderSourcePath);
    std::string fragmentShaderSource = loadShader(fragmentShaderSourcePath);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        std::exit(-1);
    }

    unsigned int vertexShader =
        createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fragmentShader =
        createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    attachAndLinkShaders(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() { glUseProgram(this->_ID); }

unsigned int Shader::getID() { return _ID; }

unsigned int Shader::createAndCompileShader(unsigned int type, const char *source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    ErrorMessage(shader, type);
    return shader;
}

void Shader::attachAndLinkShaders(unsigned int vertexShader, unsigned int fragmentShader) {
    this->_ID = glCreateProgram();
    glAttachShader(this->_ID, vertexShader);
    glAttachShader(this->_ID, fragmentShader);
    glLinkProgram(this->_ID);

    int success;
    char infolog[512];
    glGetProgramiv(this->_ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infolog);
        std::cerr << "ERROR SHARDER FRAGMENT COMPILATION ERROR\n" << infolog;
        std::exit(-1);
    }
}

void Shader::ErrorMessage(unsigned int shader, unsigned int type) {
    int succeded{};
    char infolog[512];
    std::string m = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";

    glGetShaderiv(shader, GL_COMPILE_STATUS, &succeded);
    if (!succeded) {
        glGetShaderInfoLog(shader, 512, nullptr, infolog);
        std::cerr << "ERROR SHARDER " << m << " COMPILATION ERROR\n" << infolog;
        std::exit(-1);
    }
}

std::string Shader::loadShader(const std::string &name) {
    std::ifstream file(name);
    if (!file.is_open()) {
        std::cerr << "Cannot open shader file: " << name << '\n';
        std::exit(-1);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(_ID, name.c_str()), x, y, z, w);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setInt(const std::string &name, int i) const {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), i);
}
