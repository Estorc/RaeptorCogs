#include <RaeptorCogs/External/glad/glad.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/Buffer.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>

namespace RaeptorCogs::GAPI::GL {

Framebuffer::Framebuffer() {
  GLuint newID = 0;
  glGenFramebuffers(1, &newID);
  this->glObjectID = newID;
}
void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, this->getID());
}
void Framebuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Framebuffer::~Framebuffer() {
  if (this->isValid()) {
    glDeleteFramebuffers(1, &this->glObjectID);
    this->glObjectID = 0;
  }
}

Renderbuffer::Renderbuffer() {
  GLuint newID = 0;
  glGenRenderbuffers(1, &newID);
  this->glObjectID = newID;
}
void Renderbuffer::bind() {
  glBindRenderbuffer(GL_RENDERBUFFER, this->getID());
}
void Renderbuffer::unbind() const {
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
Renderbuffer::~Renderbuffer() {
  if (this->isValid()) {
    glDeleteRenderbuffers(1, &this->glObjectID);
    this->glObjectID = 0;
  }
}

Vertexbuffer::Vertexbuffer() {
  GLuint newID = 0;
  glGenBuffers(1, &newID);
  this->glObjectID = newID;
}
void Vertexbuffer::bind() {
  glBindBuffer(GL_ARRAY_BUFFER, this->getID());
}
void Vertexbuffer::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Vertexbuffer::build(const void *data, size_t size) {
  this->bind();
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
}
Vertexbuffer::~Vertexbuffer() {
  if (this->isValid()) {
    glDeleteBuffers(1, &this->glObjectID);
    this->glObjectID = 0;
  }
}

Elementbuffer::Elementbuffer() {
  GLuint newID = 0;
  glGenBuffers(1, &newID);
  this->glObjectID = newID;
}
void Elementbuffer::bind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->getID());
}
void Elementbuffer::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Elementbuffer::build(const void *data, size_t size) {
  this->bind();
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
}
Elementbuffer::~Elementbuffer() {
  if (this->isValid()) {
    glDeleteBuffers(1, &this->glObjectID);
    this->glObjectID = 0;
  }
}

ShaderStorageBuffer::ShaderStorageBuffer() {
  GLuint newID = 0;
  glGenBuffers(1, &newID);
  this->glObjectID = newID;
}

void ShaderStorageBuffer::bind() {
#ifndef __EMSCRIPTEN__
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->getID());
#endif
}
void ShaderStorageBuffer::unbind() const {
#ifndef __EMSCRIPTEN__
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif
}
ShaderStorageBuffer::~ShaderStorageBuffer() {
  if (this->isValid()) {
    glDeleteBuffers(1, &this->glObjectID);
    this->glObjectID = 0;
  }
}

void ShaderStorageBuffer::allocate(size_t size) {
#ifndef __EMSCRIPTEN__
  this->bind();
  glBufferData(
      GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
#endif
}

void ShaderStorageBuffer::setBindingPoint(unsigned int bindingPoint) {
#ifndef __EMSCRIPTEN__
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, this->getID());
#endif
}

void ShaderStorageBuffer::upload(const void *data, size_t size, size_t offset) {
#ifndef __EMSCRIPTEN__
  this->bind();
  glBufferSubData(
      GL_SHADER_STORAGE_BUFFER,
      static_cast<GLintptr>(offset),
      static_cast<GLsizeiptr>(size),
      data);
#endif
}

} // namespace RaeptorCogs::GAPI::GL