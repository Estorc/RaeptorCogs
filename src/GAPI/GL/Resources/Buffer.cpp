#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/GL/Resources/Buffer.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::GL {

void Framebuffer::initialize() {
    GLuint newID = 0;
    glGenFramebuffers(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteFramebuffers(1, p);
        delete p;
    });
}
void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->getID());
}
void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderbuffer::initialize() {
    GLuint newID = 0;
    glGenRenderbuffers(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteRenderbuffers(1, p);
        delete p;
    });
}

void Renderbuffer::bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, this->getID());
}
void Renderbuffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}




void Vertexbuffer::initialize() {
    GLuint newID = 0;
    glGenBuffers(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteBuffers(1, p);
        delete p;
    });
}
void Vertexbuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->getID());
}
void Vertexbuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





void Elementbuffer::initialize() {
    GLuint newID = 0;
    glGenBuffers(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteBuffers(1, p);
        delete p;
    });
}
void Elementbuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->getID());
}
void Elementbuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ShaderStorageBuffer::initialize() {
    GLuint newID = 0;
    glGenBuffers(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteBuffers(1, p);
        delete p;
    });
}
void ShaderStorageBuffer::bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->getID());
}
void ShaderStorageBuffer::unbind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

}