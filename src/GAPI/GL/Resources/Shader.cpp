#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/GL/Resources/Shader.hpp>
#include <iostream>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::GL {

/*Shader::Shader(const char* vertexSource, const char* fragmentSource) {
    this->vertexSource = vertexSource;
    this->fragmentSource = fragmentSource;
}*/

void Shader::initialize() {

}

void Shader::build(const char* vertexSource, const char* fragmentSource) {
    if (this->isValid()) {
        glDeleteProgram(this->getID());
        this->id = nullptr;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    GLuint new_id = glCreateProgram();
    id = std::shared_ptr<GLuint>(new GLuint(new_id), [](GLuint* p){
        glDeleteProgram(*p);
        delete p;
    });
    glAttachShader(this->getID(), vertexShader);
    glAttachShader(this->getID(), fragmentShader);
    glLinkProgram(this->getID());

    glGetProgramiv(this->getID(), GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->getID(), 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(this->getID());
        this->id = nullptr;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void Shader::bind() {
    glUseProgram(this->getID());
}

void Shader::unbind() const {
    glUseProgram(0);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->getID(), name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->getID(), name.c_str()), value);
}

void Shader::setUInt(const std::string &name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(this->getID(), name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->getID(), name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(this->getID(), name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(this->getID(), name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(this->getID(), name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(this->getID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(this->getID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(this->getID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

}