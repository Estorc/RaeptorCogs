#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/Core/Platform.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>
#include <RaeptorCogs/Graphics/Graphic.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

namespace RaeptorCogs::GAPI::GL {

void GraphicCore::initGraphics() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    Window* window = new Window(800, 600, "", nullptr, nullptr, new GL::WindowContext(&this->getRenderer()), WindowFlags::CONTEXT_ONLY | WindowFlags::DESTROY_AFTER_INIT);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    window->makeContextCurrent();

    #ifndef __EMSCRIPTEN__
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    #endif
    std::cout << "GL initialized successfully." << std::endl;

    this->getRenderer().getPlatform().getWindows().push_back(window);
} 

void GraphicCore::buildShaders() {
    Common::ShaderInfo shaderInfo = {};
    shaderInfo.vertex_shader = FindEmbeddedShader("shader_main", Common::ShaderStage::VERTEX, "core");
    shaderInfo.fragment_shader = FindEmbeddedShader("shader_main", Common::ShaderStage::FRAGMENT, "core");
    this->getMainShader()->build(shaderInfo);
    shaderInfo.fragment_shader = FindEmbeddedShader("shader_mask", Common::ShaderStage::FRAGMENT, "core");
    this->getMaskShader()->build(shaderInfo);
}

void GraphicCore::buildBuffers() {

    this->buildShaders();
    std::vector<Vertex2D> quadVertices = {
        {{0.0f, 0.0f},    {0.0f, 0.0f}},
        {{1.0f, 0.0f},    {1.0f, 0.0f}},
        {{1.0f, 1.0f},    {1.0f, 1.0f}},
        {{0.0f, 1.0f},    {0.0f, 1.0f}}
    };
    std::vector<Vertex2D> circleVertices;
    const int CIRCLE_SEGMENTS = 64;
    for (int i = 0; i <= CIRCLE_SEGMENTS; ++i) {
        float angle = static_cast<float>(i) * 2.0f * glm::pi<float>() / static_cast<float>(CIRCLE_SEGMENTS);
        circleVertices.push_back({{0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)}, {0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)}});
    }



    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        2, 3, 0
    };
    std::vector<unsigned int> circleIndices;
    for (int i = 1; i <= CIRCLE_SEGMENTS; ++i) {
        circleIndices.push_back(0);
        circleIndices.push_back(static_cast<unsigned int>(i));
        circleIndices.push_back(static_cast<unsigned int>(i + 1));
    }


    std::vector<Vertex2D> shapesVertices;
    shapesVertices.insert(shapesVertices.end(), quadVertices.begin(), quadVertices.end());
    shapesVertices.insert(shapesVertices.end(), circleVertices.begin(), circleVertices.end());
    std::vector<unsigned int> shapesIndices;
    shapesIndices.insert(shapesIndices.end(), quadIndices.begin(), quadIndices.end());
    shapesIndices.insert(shapesIndices.end(), circleIndices.begin(), circleIndices.end());
    /*this->renderbuffer->bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->windows.front()->getWidth(), this->windows.front()->getHeight());
    this->renderbuffer->unbind();*/

    // Vertex buffer
    this->getQuadVBO()->bind();
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vertex2D) * shapesVertices.size()), shapesVertices.data(), GL_STATIC_DRAW);

    // Element buffer
    this->getQuadEBO()->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(unsigned int) * shapesIndices.size()), shapesIndices.data(), GL_STATIC_DRAW);
}

GLuint GraphicCore::getMaxTextureUnits() const {
    static GLint maxTextureUnits = 0;
    if (maxTextureUnits == 0) {
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    }
    return static_cast<GLuint>(maxTextureUnits);
}


void GraphicCore::enableStencilGuarding() {
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
}

void GraphicCore::drawElementsInstancedBaseVertexBaseInstance(size_t count, size_t instanceCount, size_t first, int baseVertex, unsigned int baseInstance) {
    (void) first;
    #ifndef __EMSCRIPTEN__
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)), static_cast<GLsizei>(instanceCount), baseVertex, baseInstance);
    #endif
}
 

void GraphicCore::bindGraphicTexture(Graphic2D& graphic) {
    glActiveTexture(GL_TEXTURE0);
    graphic.bind();
}

void GraphicCore::bindMaskTexture() {
    glActiveTexture(GL_TEXTURE0 + this->getMaxTextureUnits() - 1);
    glBindTexture(GL_TEXTURE_2D, this->maskTextures.second->getID());
}

void GraphicCore::setTextureUniform(ObjectHandler<Common::Shader> shader) {
    shader->setInt("uTextureSampler", 0);
}

void GraphicCore::setMaskTextureUniform(ObjectHandler<Common::Shader> shader) {
    shader->setInt("uMaskTextureSampler", static_cast<int>(this->getMaxTextureUnits() - 1));
}

void GraphicCore::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, abs(height));
}

void GraphicCore::clearSwapchainBuffers() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GraphicCore::useBlend() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicCore::setRenderTarget(Window* window, int , int , int , int ) {
    glDisable(GL_SCISSOR_TEST);
    if (window) {
        window->makeContextCurrent();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GraphicCore::setRenderTarget(Texture texture, int x, int y, int width, int height) {
    this->getRenderer().getPlatform().getWindows().front()->makeContextCurrent();
    this->getTextureFramebuffer()->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer not complete! (" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << ")" << std::endl;
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, abs(height));
}

void GraphicCore::clearColorBuffer(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}


}