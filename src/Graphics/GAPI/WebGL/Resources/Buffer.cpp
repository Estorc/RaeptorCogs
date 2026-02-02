#ifdef __EMSCRIPTEN__
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Resources/Buffer.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::WebGL {

void ShaderStorageBuffer::initialize() {
    GLuint newID = 0;
    glGenTextures(1, &newID);
    this->id = std::shared_ptr<GLuint>(new GLuint(newID), [](GLuint* p){
        glDeleteTextures(1, p);
        delete p;
    });
    GLuint doubleBufID = 0;
    glGenTextures(1, &doubleBufID);
    this->doubleBuffer = std::shared_ptr<GLuint>(new GLuint(doubleBufID), [](GLuint* p){
        glDeleteTextures(1, p);
        delete p;
    });
}
void ShaderStorageBuffer::bind() {
    glBindTexture(GL_TEXTURE_2D, this->getID());
}
void ShaderStorageBuffer::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

constexpr size_t UINT_SIZE = 4; // RGBA32UI has 4 bytes per uint
constexpr size_t BYTES_PER_VEC4 = 4 * UINT_SIZE; // 4 uints per vec4

void ShaderStorageBuffer::allocate(size_t size) {
    this->bind();

    size_t texWidth = IDATATEX_WIDTH;
    size_t texHeight = (size + (texWidth * BYTES_PER_VEC4 - 1)) / (texWidth * BYTES_PER_VEC4);
    this->height = texHeight;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, static_cast<GLsizei>(texWidth), static_cast<GLsizei>(texHeight));

    glBindTexture(GL_TEXTURE_2D, *this->doubleBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, static_cast<GLsizei>(texWidth), static_cast<GLsizei>(texHeight));

    std::cout << "Allocated Shader Storage Buffer of size " << size << " bytes (" << texWidth << "x" << texHeight << " texture)." << std::endl;
}

void ShaderStorageBuffer::setBindingPoint(unsigned int bindingPoint) {
    this->bindingPoint = bindingPoint + 4; // Reserve first 4 texture units for other uses
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->bindingPoint));
    this->bind();
    glActiveTexture(GL_TEXTURE0);
}

void ShaderStorageBuffer::upload(const void* data, size_t size, size_t offset) {
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->bindingPoint));
    this->bind();
    // WHY TWO ADDITIONAL VEC4???
    size_t vec4Count = size / BYTES_PER_VEC4 + 2;
    size_t vec4Offset = offset / BYTES_PER_VEC4;
    data = static_cast<const char*>(data) - (offset % BYTES_PER_VEC4);
    size_t texWidth = IDATATEX_WIDTH;

    while (vec4Count > 0) {
        size_t x = vec4Offset % texWidth;
        size_t y = vec4Offset / texWidth;

        size_t rowSpace = texWidth - x;
        size_t n = std::min(rowSpace, vec4Count);

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,          // MUST be 0
            static_cast<GLint>(x), static_cast<GLint>(y),
            static_cast<GLsizei>(n), 1,
            GL_RGBA_INTEGER,
            GL_UNSIGNED_INT,
            data
        );

        vec4Offset += n;
        vec4Count  -= n;
        data = static_cast<const char*>(data) + n * BYTES_PER_VEC4;
    }
    glActiveTexture(GL_TEXTURE0);
}

}
#endif