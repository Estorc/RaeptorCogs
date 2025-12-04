#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/GL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/Vertex.hpp>
#include <RaeptorCogs/Window.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::GL {

    void WindowContext::initialize(Window* window) {
        glfwMakeContextCurrent(window->getGLFWWindow());
        this->buildVAO();
        glfwSwapInterval(0);
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
        #ifndef __EMSCRIPTEN__
        backend.getGraphicCore().getStaticInstanceDataSSBO()->bind();
        glBufferData(GL_SHADER_STORAGE_BUFFER, STATIC_INSTANCE_DATA_SIZE, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, backend.getGraphicCore().getStaticInstanceDataSSBO()->getID()); // binding = 1
        backend.getGraphicCore().getDynamicInstanceDataSSBO()->bind();
        glBufferData(GL_SHADER_STORAGE_BUFFER, DEFAULT_DYNAMIC_DATA_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, backend.getGraphicCore().getDynamicInstanceDataSSBO()->getID()); // binding = 2
        #else
        backend.getIDataTex()->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, IDATATEX_WIDTH, IDATATEX_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        backend.getIDataTex()->unbind();
        #endif
    }

}