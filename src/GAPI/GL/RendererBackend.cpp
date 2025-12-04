#include <RaeptorCogs/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <RaeptorCogs/Platform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

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

Common::WindowContext* RendererBackend::createWindowContext() {
    return new WindowContext(this);
}

void RendererBackend::render(Window* window, int x, int y, int width, int height) {
    window->makeContextCurrent();
    this->getRenderPipeline().renderMask(window, x, y, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->getRenderPipeline().renderPass(x, y, width, height);
}

void RendererBackend::render(Texture& texture, int x, int y, int width, int height) {
    this->getPlatform().getWindows().front()->makeContextCurrent();
    this->getRenderPipeline().renderMask(nullptr, x, y, width, height);
    this->getGraphicCore().getTextureFramebuffer()->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer not complete! (" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << ")" << std::endl;
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
    this->getRenderPipeline().renderPass(x, y, width, -height);
    glDisable(GL_SCISSOR_TEST);
}


bool RendererBackend::isInitialized() const {
    return !this->getPlatform().getWindows().empty();
}

};