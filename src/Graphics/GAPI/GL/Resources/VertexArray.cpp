#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Resources/VertexArray.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::GL {

VertexArray::VertexArray() {
    GLuint newID = 0;
    glGenVertexArrays(1, &newID);
    this->glObjectID = newID;
}

VertexArray::~VertexArray() {
    if (this->isValid()) {
        glDeleteVertexArrays(1, &this->glObjectID);
        this->glObjectID = 0;
    }
}

void VertexArray::bind() {
    glBindVertexArray(this->getID());
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

}