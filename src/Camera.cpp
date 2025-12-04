#include <RaeptorCogs/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace RaeptorCogs {

Camera2D::Camera2D() = default;
Camera2D::~Camera2D() = default;

void Camera2D::rebuildViewMatrix() {
    if (!(flags & CameraFlags::NEEDS_REBUILD_VIEW_MATRIX)) {
        return;
    }
    viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
    flags &= ~CameraFlags::NEEDS_REBUILD_VIEW_MATRIX;
}
void Camera2D::rebuildProjectionMatrix() {
    if (!(flags & CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX)) {
        return;
    }
    projectionMatrix = glm::ortho(-1.0f * zoom, 1.0f * zoom, -1.0f * zoom, 1.0f * zoom, -1000.0f, 1000.0f);
    flags &= ~CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
}

glm::mat4 Camera2D::getViewMatrix() {
    this->rebuildViewMatrix();
    return viewMatrix;
}

glm::mat4 Camera2D::getProjectionMatrix() {
    this->rebuildProjectionMatrix();
    return projectionMatrix;
}

void Camera2D::setPosition(const glm::vec2 &pos) {
    position = pos;
    flags |= CameraFlags::NEEDS_REBUILD_VIEW_MATRIX;
}

void Camera2D::setZoom(float z) {
    zoom = z;
    flags |= CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
}

float Camera2D::getPositionX() const {
    return position.x;
}

float Camera2D::getPositionY() const {
    return position.y;
}

glm::vec2 Camera2D::getPosition() const {
    return position;
}

float Camera2D::getZoom() const {
    return zoom;
}

void Camera2D::update(GAPI::Common::RenderPipeline& pipeline) {
    GAPI::Common::FrameData& frameData = pipeline.getFrameData();
    frameData.viewMatrix = this->getViewMatrix();
    frameData.projectionMatrix = glm::scale(this->getProjectionMatrix(), glm::vec3(2.0f / static_cast<float>(frameData.width), -2.0f / static_cast<float>(frameData.height), 1.0f));
}

}