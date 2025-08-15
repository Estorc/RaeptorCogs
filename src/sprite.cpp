#include <RaeptorCogs/Sprite.hpp>
#include <iostream>
namespace RaeptorCogs {

Sprite::Sprite(Texture &texture) : texture(&texture) {}


void Sprite::addToRenderer(Singletons::Renderer &renderer) {
    // Add to renderer's batch
    renderer.addGraphic(this);
}

void Sprite::computeInstanceData(InstanceData &data, std::vector<float> &instanceDataBuffer) {
    data.model = this->getModelMatrix();
    data.uvRect = texture ? texture->getUVRect() : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    data.type = RENDERER_MODE_2D_SPRITE;

    glm::vec3 color = this->getColor();
    data.dataOffset = instanceDataBuffer.size(); // Offset into the instance data buffer
    instanceDataBuffer.insert(instanceDataBuffer.end(), reinterpret_cast<float*>(&color), reinterpret_cast<float*>(&color) + sizeof(color) / sizeof(float));
}

void Sprite::bind() const {
    if (texture) {
        texture->bind();
    }
}


void Sprite::setTexture(Texture &texture) {
    bool needChangeGraphicPosition = (this->getID() != texture.getID());
    this->texture = &texture;
    if (needChangeGraphicPosition) {
        this->renderer->changeGraphicPosition(this);
    }
}


Texture* Sprite::getTexture() const {
    return texture;
}

GLuint Sprite::getID() const {
    return texture ? texture->getID() : 0;
}

bool Sprite::isOpaque() const {
    return texture ? texture->isOpaque() : true;
}

}