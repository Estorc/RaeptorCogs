#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace RaeptorCogs {

#pragma region Graphic

Graphic2D::~Graphic2D() {
    if (renderer) {
        renderer->remove(*this); // Remove from renderer if it exists
    }
}

void Graphic2D::setRenderer(Singletons::Renderer* renderer) {
    this->renderer = renderer;
}

Singletons::Renderer* Graphic2D::getRenderer() const {
    return this->renderer;
}

void Graphic2D::setZIndex(float z) {
    this->zIndex = z;
    this->updatePositionInRenderLists();
    this->setDataDirty(true);
}

float Graphic2D::getZIndex() const {
    return this->zIndex;
}

bool Graphic2D::computeInstanceData(GAPI::Common::InstanceAllocator&, ComputeInstanceDataMode) {
    throw std::runtime_error("Graphic2D::computeInstanceData must be overridden in derived classes.");
}

void Graphic2D::setReadingMaskID(int index, bool inheritFromParent) {
    if (inheritFromParent && !this->hasFlag(GraphicFlags::INHERIT_READ_MASK)) return;
    if (!inheritFromParent && index != 0) this->clearFlag(GraphicFlags::INHERIT_READ_MASK);
    this->readingMaskIndex = index;
    this->updatePositionInRenderLists();
    if (!this->getWritingMaskID())
        for (Node* child : this->getChildren()) {
            if (child->isInstanceOf<Graphic2D>()) {
                static_cast<Graphic2D*>(child)->setReadingMaskID(index, true);
            }
        }
    this->setDataDirty(true);
}

void Graphic2D::setWritingMaskID(int index) {
    this->writingMaskIndex = index;
    this->updatePositionInRenderLists();
    for (Node* child : this->getChildren()) {
        if (child->isInstanceOf<Graphic2D>()) {
            static_cast<Graphic2D*>(child)->setReadingMaskID(index, true);
        }
    }
    this->setDataDirty(true);
}

int Graphic2D::getReadingMaskID() const {
    return this->readingMaskIndex;
}

int Graphic2D::getWritingMaskID() const {
    return this->writingMaskIndex;
}

bool Graphic2D::isDataDirty() const {
    return this->hasFlag(GraphicFlags::DATA_DIRTY);
}

void Graphic2D::setDataDirty(bool dirty) {
    if (dirty) {
        this->setFlag(GraphicFlags::DATA_DIRTY);
        if (this->getRenderListCount()) {
            this->getBatchHandler().isDirty = true;
        }
    } else {
        this->clearFlag(GraphicFlags::DATA_DIRTY);
        if (this->getRenderListCount()) {
            this->getBatchHandler().isDirty = false;
        }
    }
}

void Graphic2D::setParent(Node* parent) {
    Node::setParent(parent);
    this->setReadingMaskID(parent && parent->isInstanceOf<Graphic2D>() ? (static_cast<Graphic2D*>(parent)->getWritingMaskID() ? static_cast<Graphic2D*>(parent)->getWritingMaskID() : static_cast<Graphic2D*>(parent)->getReadingMaskID()) : 0, true);
}

GAPI::Common::GraphicBatchHandler &Graphic2D::getBatchHandler() {
    return RaeptorCogs::Renderer().getBackend().getRenderPipeline().getBatchHandlerAt(this->batchHandlerCursor);
}

void Graphic2D::updatePositionInRenderLists() {
    if (!this->getRenderListCount()) return;
    auto& handler = this->getBatchHandler();
    handler.rendererKey = this->buildRendererKey();
    for (auto& renderList : this->getRenderLists()) {
        renderList->markDirty(handler);
    }
}

#pragma endregion
#pragma region RenderableGraphics2D

void RenderableGraphic2D::setColor(const glm::vec3 &color) {
    this->color = color;
    this->setDataDirty(true);
    this->setGlobalColorDirty(true);
}

void RenderableGraphic2D::setVisibility(bool visible) {
    if (visible) {
        FlagSet<RenderableGraphicFlags>::setFlag(RenderableGraphicFlags::IS_VISIBLE);
    } else {
        FlagSet<RenderableGraphicFlags>::clearFlag(RenderableGraphicFlags::IS_VISIBLE);
    }
    this->setDataDirty(true);
}

bool RenderableGraphic2D::isVisible() const {
    return FlagSet<RenderableGraphicFlags>::hasFlag(RenderableGraphicFlags::IS_VISIBLE);
}

glm::vec3 RenderableGraphic2D::getColor() const {
    return color;
}

Shape &RenderableGraphic2D::getShape() const {
    return *shape;
}

void RenderableGraphic2D::rebuildGlobalColor() {
    if (!this->isGlobalColorDirty()) {
        return;
    }
    if (this->getParent() && this->getParent()->isInstanceOf<RenderableGraphic2D>()) {
        globalColor = (((static_cast<RenderableGraphic2D*>(this->getParent()))->getGlobalColor()) * color);
    } else {
        globalColor = color;
    }
    this->setGlobalColorDirty(false);
}

glm::vec3 RenderableGraphic2D::getGlobalColor() {
    this->rebuildGlobalColor();
    return globalColor;
}

void RenderableGraphic2D::setGlobalColorDirty(bool dirty) {
    if (dirty) {
        FlagSet<RenderableGraphicFlags>::setFlag(RenderableGraphicFlags::GLOBAL_COLOR_DIRTY);
        this->setDataDirty(true);
        for (Node* child : this->getChildren()) {
            if (child->isInstanceOf<RenderableGraphic2D>()) {
                static_cast<RenderableGraphic2D*>(child)->setGlobalColorDirty(true);
            }
        }
    } else {
        FlagSet<RenderableGraphicFlags>::clearFlag(RenderableGraphicFlags::GLOBAL_COLOR_DIRTY);
    }
}

bool RenderableGraphic2D::isGlobalColorDirty() const {
    return FlagSet<RenderableGraphicFlags>::hasFlag(RenderableGraphicFlags::GLOBAL_COLOR_DIRTY);
}

void RenderableGraphic2D::setParent(Node* parent) {
    Graphic2D::setParent(parent);
    this->setGlobalColorDirty(true);
}

#pragma endregion
#pragma region TransformableGraphic2D


glm::mat4 &TransformableGraphic2D::getLocalMatrix() {
    return localMatrix;
}

glm::mat4 &TransformableGraphic2D::getGlobalMatrix() {
    return globalMatrix;
}

glm::mat4 TransformableGraphic2D::getModelMatrix() {
    this->rebuildLocalMatrix();
    this->rebuildGlobalMatrix();
    return globalMatrix;
}

void TransformableGraphic2D::setLocalMatrixDirty(bool dirty) {
    if (dirty) {
        FlagSet<TransformFlags>::setFlag(TransformFlags::LOCAL_MATRIX_DIRTY);
        this->setDataDirty(true);
        for (Node* child : this->getChildren()) {
            if (child->isInstanceOf<TransformableGraphic2D>()) {
                static_cast<TransformableGraphic2D*>(child)->setGlobalMatrixDirty(true);
            }
        }
    } else {
        FlagSet<TransformFlags>::clearFlag(TransformFlags::LOCAL_MATRIX_DIRTY);
    }
}

bool TransformableGraphic2D::isLocalMatrixDirty() const {
    return FlagSet<TransformFlags>::hasFlag(TransformFlags::LOCAL_MATRIX_DIRTY);
}

void TransformableGraphic2D::setGlobalMatrixDirty(bool dirty) {
    if (dirty) {
        FlagSet<TransformFlags>::setFlag(TransformFlags::GLOBAL_MATRIX_DIRTY);
        this->setDataDirty(true);
        for (Node* child : this->getChildren()) {
            if (child->isInstanceOf<TransformableGraphic2D>()) {
                static_cast<TransformableGraphic2D*>(child)->setGlobalMatrixDirty(true);
            }
        }
    } else {
        FlagSet<TransformFlags>::clearFlag(TransformFlags::GLOBAL_MATRIX_DIRTY);
    }
}

bool TransformableGraphic2D::isGlobalMatrixDirty() const {
    return FlagSet<TransformFlags>::hasFlag(TransformFlags::GLOBAL_MATRIX_DIRTY);
}




void TransformableGraphic2D::rebuildLocalMatrix() {
    if (!this->isLocalMatrixDirty()) {
        return;
    }

    // Rebuild the model matrix
    localMatrix = glm::mat4(1.0f);
    localMatrix = glm::translate(localMatrix, glm::vec3(position, 0.0f));
    localMatrix = glm::rotate(localMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    localMatrix = glm::scale(localMatrix, glm::vec3(size, 1.0f));
    localMatrix = glm::scale(localMatrix, glm::vec3(scale, 1.0f));
    localMatrix = glm::translate(localMatrix, glm::vec3(-anchor, 0.0f));
    localMatrix[3][2] = this->getZIndex()/1000.0f; // Restore zIndex
}

void TransformableGraphic2D::rebuildGlobalMatrix() {
    if (!this->isGlobalMatrixDirty() && !this->isLocalMatrixDirty()) {
        return;
    }
    if (this->getParent() && this->getParent()->isInstanceOf<TransformableGraphic2D>()) {
        TransformableGraphic2D* parent = static_cast<TransformableGraphic2D*>(this->getParent());
        glm::mat4 parentMatrix = parent->getModelMatrix();
        parentMatrix = glm::translate(parentMatrix, glm::vec3(parent->getAnchor(), 0.0f));
        parentMatrix = glm::scale(parentMatrix, glm::vec3(glm::vec2(1.0f) / parent->getSize(), 1.0f));
        globalMatrix = parentMatrix * localMatrix;
    } else {
        globalMatrix = localMatrix;
    }
    this->setGlobalMatrixDirty(false);
    this->setLocalMatrixDirty(false);
}

void TransformableGraphic2D::setPosition(const glm::vec2 &pos) {
    this->position = pos;
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setSize(const glm::vec2 &size) {
    this->size = size;
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setScale(const glm::vec2 &scale) {
    this->scale = scale;
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setRotation(float angle) {
    this->rotation = angle;
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setAnchor(const glm::vec2 &anchor) {
    this->anchor = anchor;
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setZIndex(float z) {
    Graphic2D::setZIndex(z);
    this->setLocalMatrixDirty(true);
}

void TransformableGraphic2D::setParent(Node* parent) {
    RenderableGraphic2D::setParent(parent);
    this->setGlobalMatrixDirty(true);
}



glm::vec2 TransformableGraphic2D::getPosition() const {
    return position;
}

glm::vec2 TransformableGraphic2D::getSize() const {
    return size;
}

glm::vec2 TransformableGraphic2D::getScale() const {
    return scale;
}

float TransformableGraphic2D::getRotation() const {
    return rotation;
}

glm::vec2 TransformableGraphic2D::getAnchor() const {
    return anchor;
}

#pragma endregion

}