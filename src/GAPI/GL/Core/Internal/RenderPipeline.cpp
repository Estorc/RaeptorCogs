#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/Platform.hpp>
#include <RaeptorCogs/Graphic.hpp>

#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <RaeptorCogs/Camera.hpp>

namespace RaeptorCogs::GAPI::GL {

void RenderPipeline::beginFrame() {
    //
}

void RenderPipeline::endFrame() {
    for (Window *window : this->getRenderer().getPlatform().getWindows()) {
        window->makeContextCurrent();

        this->getRenderer().getImGuiModule().RenderImGuiDrawData();

        glfwSwapBuffers(window->getGLFWWindow());
    }
}



void RenderPipeline::renderPass(int x, int y, int width, int height) {
    GraphicCore &graphicCore = static_cast<GraphicCore&>(this->getRenderer().getGraphicCore());
    this->useNormalRenderList();
    this->beginBatch(x, y, width, height, graphicCore.getMainShader());
    this->processBatch();
    this->flushBatch();
}

void RenderPipeline::renderMask(Window* window, int x, int y, int width, int height) {
    GraphicCore &graphicCore = static_cast<GraphicCore&>(this->getRenderer().getGraphicCore());
    this->useMaskRenderList();
    graphicCore.getRenderbuffer()->bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    graphicCore.getRenderbuffer()->unbind();
    for (int i = 0; i < 2; i++) {
        graphicCore.getMaskTextures().first->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        graphicCore.getPingPongMaskFramebuffer().first->bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, graphicCore.getRenderbuffer()->getID());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, graphicCore.getMaskTextures().first->getID(), 0);
        GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Error: Framebuffer not complete! (" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << ")" << std::endl;
        }
        GLuint clearVal = 0;
        glClearBufferuiv(GL_COLOR, 0, &clearVal); 

        glClearColor(0, 0, 0, 1);
        std::swap(graphicCore.getMaskTextures().first, graphicCore.getMaskTextures().second);
        std::swap(graphicCore.getPingPongMaskFramebuffer().first, graphicCore.getPingPongMaskFramebuffer().second);
    }

    glEnable(GL_STENCIL_TEST);
    this->beginBatch(x, y, width, height, graphicCore.getMaskShader());
    this->processBatch([x, y, width, height, &graphicCore]() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, graphicCore.getPingPongMaskFramebuffer().second->getID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, graphicCore.getPingPongMaskFramebuffer().first->getID());
        glBlitFramebuffer(x, y, x + width, y + height,
                        x, y, x + width, y + height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
        std::swap(graphicCore.getMaskTextures().first, graphicCore.getMaskTextures().second);
        std::swap(graphicCore.getPingPongMaskFramebuffer().first, graphicCore.getPingPongMaskFramebuffer().second);
    });
    this->flushBatch();
    glDisable(GL_STENCIL_TEST);
    //std::swap(this->maskTextures.first, this->maskTextures.second);
    //std::swap(this->pingPongMaskFramebuffer.first, this->pingPongMaskFramebuffer.second);

    if (window && window->isMouseInWindow()) {
        uint32_t pixel;
        glm::ivec2 mousePos = window->getMousePosition();
        glm::ivec2 windowSize = window->getSize();
        mousePos.y = windowSize.y - mousePos.y; // Invert Y coordinate
        glReadPixels(mousePos.x, mousePos.y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);
        RaeptorCogs::Mouse().setHoveredData(pixel);
    }
}

}