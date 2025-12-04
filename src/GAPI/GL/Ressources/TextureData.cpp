#include <RaeptorCogs/GAPI/GL/Ressources/TextureData.hpp>
#include <iostream>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::GL {

#pragma region TextureData

void TextureData::initialize() {
    if (this->isValid()) {
        glDeleteTextures(1, this->id.get());
        this->id = nullptr;
    }
    GLuint newID = 0;
    glGenTextures(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteTextures(1, p);
        delete p;
    });
}

void TextureData::bind() {
    glBindTexture(GL_TEXTURE_2D, this->getID());
}

void TextureData::build(int width, int height, void * data, GLenum minFilter, GLenum magFilter) {
    this->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
    #ifndef __EMSCRIPTEN__
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);
    #endif

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void TextureData::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion

}