#ifdef __EMSCRIPTEN__
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>
#include <RaeptorCogs/Graphics/Window.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>

namespace RaeptorCogs::GAPI::WebGL {

    void WindowContext::initialize(Window* window) {
        glfwMakeContextCurrent(window->getGLFWWindow());
        this->buildVAO();
    }

}
#endif