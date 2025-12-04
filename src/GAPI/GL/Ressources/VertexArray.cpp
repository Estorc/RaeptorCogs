#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/GL/Ressources/VertexArray.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::GL {

void VertexArray::initialize() {
    GLuint newID = 0;
    glGenVertexArrays(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteVertexArrays(1, p);
        delete p;
    });
}

void VertexArray::bind() {
    glBindVertexArray(this->getID());
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

}