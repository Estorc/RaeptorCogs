#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace RaeptorCogs {

Graphic::~Graphic() {
    if (renderer) {
        renderer->removeGraphic(this); // Remove from renderer if it exists
    }
}

void Graphic::setRenderer(Singletons::Renderer* renderer) {
    this->renderer = renderer;
}

void Graphic::setRendererKey(BatchKey key) {
    this->rendererKey = key;
}

Singletons::Renderer* Graphic::getRenderer() const {
    return this->renderer;
}

BatchKey Graphic::getRendererKey() const {
    return this->rendererKey;
}

void Graphic::setZIndex(float z) {
    this->zIndex = z;
    if (this->renderer) {
        this->renderer->changeGraphicPosition(this);
    }
}

float Graphic::getZIndex() const {
    return this->zIndex;
}

void TransformableGraphic2D::rebuild() {
    if (!(flags & GraphicFlags::NEEDS_REBUILD)) {
        return;
    }
    // Set the rebuild flag
    flags &= ~GraphicFlags::NEEDS_REBUILD;

    // Rebuild the model matrix
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position, zIndex));
    modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-anchor * size, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size, 1.0f));
}

void TransformableGraphic2D::setPosition(const glm::vec2 &pos) {
    this->position = pos;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
}

void TransformableGraphic2D::setSize(const glm::vec2 &size) {
    this->size = size;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
}

void TransformableGraphic2D::setRotation(float angle) {
    this->rotation = angle;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
}

void TransformableGraphic2D::setColor(const glm::vec3 &color) {
    this->color = color;
}

void TransformableGraphic2D::setAnchor(const glm::vec2 &anchor) {
    this->anchor = anchor;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
}

void TransformableGraphic2D::setVisibility(bool visible) {
    if (visible) {
        this->flags |= GraphicFlags::IS_VISIBLE;
    } else {
        this->flags &= ~GraphicFlags::IS_VISIBLE;
    }
}

bool TransformableGraphic2D::isVisible() const {
    return any(this->flags & GraphicFlags::IS_VISIBLE);
}

bool TransformableGraphic2D::needsRebuild() const {
    return any(this->flags & GraphicFlags::NEEDS_REBUILD);
}

glm::vec2 TransformableGraphic2D::getPosition() const {
    return position;
}

glm::vec2 TransformableGraphic2D::getSize() const {
    return size;
}

float TransformableGraphic2D::getRotation() const {
    return rotation;
}

glm::vec3 TransformableGraphic2D::getColor() const {
    return color;
}

glm::vec2 TransformableGraphic2D::getAnchor() const {
    return anchor;
}

glm::mat4 TransformableGraphic2D::getModelMatrix() {
    this->rebuild();
    return modelMatrix;
}

void TransformableGraphic2D::setZIndex(float z) {
    Graphic::setZIndex(z);
    this->flags |= GraphicFlags::NEEDS_REBUILD; // Ensure rebuild on z-index change
}

}