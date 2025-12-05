#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/Shader.hpp>
#include <iostream>

namespace RaeptorCogs::GAPI::Vulkan {

/*Shader::Shader(const char* vertexSource, const char* fragmentSource) {
    this->vertexSource = vertexSource;
    this->fragmentSource = fragmentSource;
}*/

void Shader::initialize() {

}

void Shader::build(const char* vertexSource, const char* fragmentSource) {
    (void)vertexSource;
    (void)fragmentSource;
}


void Shader::bind() {

}

void Shader::unbind() const {

}

void Shader::setBool(const std::string &name, bool value) const {
    (void)name;
    (void)value;
}

void Shader::setInt(const std::string &name, int value) const {
    (void)name;
    (void)value;
}

void Shader::setUInt(const std::string &name, unsigned int value) const {
    (void)name;
    (void)value;
}

void Shader::setFloat(const std::string &name, float value) const {
    (void)name;
    (void)value;
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    (void)name;
    (void)value;
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    (void)name;
    (void)value;
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    (void)name;
    (void)value;
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    (void)name;
    (void)mat;
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    (void)name;
    (void)mat;
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    (void)name;
    (void)mat;
}

}