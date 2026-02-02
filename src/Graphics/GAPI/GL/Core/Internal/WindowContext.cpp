#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>
#include <RaeptorCogs/Graphics/Window.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::GL {

    void WindowContext::initialize(Window* window) {
        glfwMakeContextCurrent(window->getGLFWWindow());
        this->buildVAO();
    }

    void WindowContext::bind() {
        if (this->quadVertexArray.get()) {
            this->quadVertexArray.get()->bind();
        }
    }

    GAPI::ObjectHandler<GAPI::Common::VertexArray>* WindowContext::getQuadVertexArray() {
        return &this->quadVertexArray;
    }

    void WindowContext::buildVAO() {
        RendererBackend& backend = static_cast<RendererBackend&>(*this->getRenderer());
        if (!backend.getGraphicCore().getQuadVBO().get()) return;
        backend.getGraphicCore().getQuadVBO()->bind();
        this->quadVertexArray.get()->bind();

        // Vertex attributes
        // Position (0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(offsetof(Vertex2D, position)));
        // UV (1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(offsetof(Vertex2D, uv)));

        this->quadVertexArray.get()->unbind(); 

        // Shader Storage Buffer Object (SSBO) for instance data
        backend.getGraphicCore().getStaticInstanceDataSSBO()->bind();
        backend.getGraphicCore().getStaticInstanceDataSSBO()->allocate(Common::STATIC_INSTANCE_DATA_SIZE);
        backend.getGraphicCore().getStaticInstanceDataSSBO()->setBindingPoint(1);
        backend.getGraphicCore().getDynamicInstanceDataSSBO()->bind();
        backend.getGraphicCore().getDynamicInstanceDataSSBO()->allocate(Common::DEFAULT_DYNAMIC_DATA_BUFFER_SIZE);
        backend.getGraphicCore().getDynamicInstanceDataSSBO()->setBindingPoint(2);
    }

}