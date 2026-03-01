#include <RaeptorCogs/Core/Platform.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/RendererBackend.hpp>
#include <iostream>

#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::GL {

void RendererBackend::initialize() {
  this->getGraphicCore().initGraphics();
  this->getGraphicCore().buildBuffers();
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  std::cout << "GL Renderer Backend initialized." << std::endl;
}

RendererBackend::~RendererBackend() {
  this->getRenderPipeline().clearRenderLists();
}

Common::WindowContext *RendererBackend::createWindowContext() {
  return new WindowContext(this);
}

void RendererBackend::render(Window *window, int x, int y, int width, int height) {
  this->getGraphicCore().setRenderTarget(window, x, y, width, height);
  this->getRenderPipeline().renderMask(window, x, y, width, height);
  this->getGraphicCore().setRenderTarget(window, x, y, width, height);
  this->getRenderPipeline().renderPass(x, y, width, height);
}

void RendererBackend::render(Texture &texture, int x, int y, int width, int height) {
  this->getGraphicCore().setRenderTarget(texture, x, y, width, height);
  this->getRenderPipeline().renderMask(nullptr, x, y, width, height);
  this->getGraphicCore().setRenderTarget(texture, x, y, width, height);
  this->getRenderPipeline().renderPass(x, y, width, -height);
}

bool RendererBackend::isInitialized() const {
  return !this->getPlatform().getWindows().empty();
}

}; // namespace RaeptorCogs::GAPI::GL