#include <RaeptorCogs/Sprite.hpp>
#include <iostream>
namespace RaeptorCogs {

Sprite2D::Sprite2D(Texture &texture) : texture(texture) {}

bool Sprite2D::computeInstanceData(GAPI::Common::InstanceAllocator &instanceAllocator, ComputeInstanceDataMode mode) {
    GAPI::Common::GraphicBatchHandler &batchHandler = this->getBatchHandler();

    if (mode == ComputeInstanceDataMode::FORCE_REBUILD) {
        instanceAllocator.allocate(batchHandler, 3); // RGB color
    }

    auto& staticDataBuffer = instanceAllocator.getStaticInstanceData(batchHandler.staticDataCursor);
    auto* dynamicDataBuffer = instanceAllocator.getDynamicInstanceData(batchHandler.dynamicDataCursor);

    if (this->isDataDirty() || mode == ComputeInstanceDataMode::REBUILD_TEXTURE || mode == ComputeInstanceDataMode::FORCE_REBUILD) {

        // Static instance data

        staticDataBuffer.model = this->getModelMatrix();
        staticDataBuffer.uvRect = texture ? texture->getUVRect() : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        staticDataBuffer.type = this->isVisible() ? RENDERER_MODE_2D_SPRITE : RENDERER_MODE_DEFAULT;
        staticDataBuffer.readingMaskID = this->getReadingMaskID();
        staticDataBuffer.writingMaskID = this->getWritingMaskID();
        if (mode == ComputeInstanceDataMode::FORCE_REBUILD) {
            staticDataBuffer.dataOffset = batchHandler.dynamicDataCursor; // Offset into the instance data buffer
        }

        // Dynamic instance data

        glm::vec3 color = this->getGlobalColor();
        dynamicDataBuffer[0] = color[0];
        dynamicDataBuffer[1] = color[1];
        dynamicDataBuffer[2] = color[2];
    }

    if (mode == ComputeInstanceDataMode::REBUILD_TEXTURE || this->isDataDirty()) {
        this->setDataDirty(false);
        return true;
    }
    return false;
}

void Sprite2D::bind() const {
    if (texture) {
        texture->bind();
    }
}


void Sprite2D::setTexture(Texture &texture) {
    bool needChangeGraphicPosition = (this->getID() != texture->getID());
    this->texture = texture;
    if (needChangeGraphicPosition) {
        this->updatePositionInRenderLists();
    }
    this->setDataDirty(true);
}


Texture Sprite2D::getTexture() const {
    return texture;
}

GLuint Sprite2D::getID() const {
    return texture ? texture->getID() : 0;
}

bool Sprite2D::isOpaque() const {
    return texture ? texture->isOpaque() : true;
}

bool Sprite2D::isVisible() const {
    return RenderableGraphic2D::isVisible() && texture->isLoaded();
}

}