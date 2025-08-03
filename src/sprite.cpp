#include <RaeptorLab/sprite.hpp>
#include <iostream>

Sprite::Sprite(Texture &texture) : texture(&texture) {}


void Sprite::addToRenderer(Renderer &renderer) {
    // Add to renderer's batch
    renderer.addGraphic(this);
}

void Sprite::computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) {
    data.model = this->getModelMatrix();
    data.uvRect = texture ? texture->getUVRect() : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    data.type = RENDERER_MODE_2D_SPRITE;

    glm::vec3 color = this->getColor();
    data.dataOffset = instanceDataBuffer.size() / sizeof(uint8_t); // Offset into the instance data buffer
    instanceDataBuffer.insert(instanceDataBuffer.end(), reinterpret_cast<uint8_t*>(&color), reinterpret_cast<uint8_t*>(&color) + sizeof(color));
}

void Sprite::bind() const {
    if (texture) {
        texture->bind();
    }
}


void Sprite::setTexture(Texture &texture) {
    if (this->texture->getID() != texture.getID()) {
        this->renderer->changeGraphicPosition(this, texture.getID());
    }
    this->texture = &texture;
}


Texture* Sprite::getTexture() const {
    return texture;
}

GLuint Sprite::getID() const {
    return texture ? texture->getID() : 0;
}