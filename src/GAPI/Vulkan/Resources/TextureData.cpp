#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Resources/TextureData.hpp>

namespace RaeptorCogs::GAPI::Vulkan {

#pragma region TextureData

void TextureData::initialize() {

}

void TextureData::bind() {

}

void TextureData::build(int width, int height, void * data, GLenum minFilter, GLenum magFilter) {
    (void)width;
    (void)height;
    (void)data;
    (void)minFilter;
    (void)magFilter;
}

void TextureData::unbind() const {

}

#pragma endregion

}