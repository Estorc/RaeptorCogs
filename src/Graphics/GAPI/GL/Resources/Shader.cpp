#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/Shader.hpp>
#include <iostream>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <RaeptorCogs/EmbedShaders.hpp>

namespace RaeptorCogs::GAPI::GL {

Shader::Shader() {

}

void Shader::build(const char* vertexSource, const char* fragmentSource) {
    if (this->isValid()) {
        glDeleteProgram(this->getID());
        this->glObjectID = 0;
    }

    if (!vertexSource || !fragmentSource) {
        std::cerr << "ERROR::SHADER::BUILD::SOURCE_CODE_NULL" << std::endl;
        return;
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
    this->glObjectID = new_id;
    glAttachShader(this->getID(), vertexShader);
    glAttachShader(this->getID(), fragmentShader);
    glLinkProgram(this->getID());

    glGetProgramiv(this->getID(), GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->getID(), 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(this->getID());
        this->glObjectID = 0;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (this->isValid()) {
        glDeleteProgram(this->glObjectID);
        this->glObjectID = 0;
    }
}

void Shader::build(const Common::ShaderInfo &shader) {
    if (!shader.vertex_shader || !shader.fragment_shader) {
        std::cerr << "ERROR::SHADER::BUILD::SHADER_INFO_INVALID" << std::endl;
        return;
    }
    rgsl_shader_blob vertexBlob = *static_cast<const rgsl_shader_blob*>(shader.vertex_shader);
    rgsl_shader_blob fragmentBlob = *static_cast<const rgsl_shader_blob*>(shader.fragment_shader);
    if (vertexBlob.stage != RGSL_VERTEX) {
        std::cerr << "ERROR::SHADER::BUILD::INVALID_VERTEX_SHADER_STAGE" << std::endl;
        return;
    }
    if (fragmentBlob.stage != RGSL_FRAGMENT) {
        std::cerr << "ERROR::SHADER::BUILD::INVALID_FRAGMENT_SHADER_STAGE" << std::endl;
        return;
    }
    this->build(static_cast<const char*>(vertexBlob.glsl_code), static_cast<const char*>(fragmentBlob.glsl_code));
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

static const std::vector<std::pair<Common::ShaderStage, size_t>> STAGES_MAP = {
    {Common::ShaderStage::VERTEX, RGSL_VERTEX},
    {Common::ShaderStage::FRAGMENT, RGSL_FRAGMENT},
    {Common::ShaderStage::GEOMETRY, RGSL_UNKNOWN_STAGE},
    {Common::ShaderStage::COMPUTE, RGSL_COMPUTE},
    {Common::ShaderStage::TESSELLATION_CONTROL, RGSL_UNKNOWN_STAGE},
    {Common::ShaderStage::TESSELLATION_EVALUATION, RGSL_UNKNOWN_STAGE}
};

const void * FindEmbeddedShader(const std::string& name, Common::ShaderStage stage, const std::string& profile) {
    return static_cast<const void *>(Common::FindEmbeddedShader(rgsl_shaders, sizeof(rgsl_shaders[0]), sizeof(rgsl_shaders) / sizeof(rgsl_shaders[0]), name, stage, STAGES_MAP, profile));
}

}