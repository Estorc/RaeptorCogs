#include <RaeptorCogs/Platform.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Window.hpp>

namespace RaeptorCogs::Singletons {

Window* Platform::createWindow(int width, int height, const std::string_view& title) {
    if (!this->renderer) {
        std::cerr << "RendererBackend::createWindow called before renderer is initialized." << std::endl;
        return nullptr;
    }
    windows.push_back(new Window(width, height, title.data(), nullptr, this->windows.front()->getGLFWWindow(), this->renderer->getBackend().createWindowContext()));
    if (!windows.back()) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return nullptr;
    }
    if (windows.front()->hasFlag(WindowFlags::DESTROY_AFTER_INIT)) {
        delete windows.front();
        windows.erase(windows.begin());
    }
    windows.back()->makeContextCurrent();
    return windows.back();
}

void Platform::destroyWindow(Window* window) {
    if (!this->renderer) {
        std::cerr << "RendererBackend::destroyWindow called before renderer is initialized." << std::endl;
        return;
    }
    if (!window) {
        return;
    }
    auto it = std::find(this->windows.begin(), this->windows.end(), window);
    if (it != this->windows.end()) {
        this->windows.erase(it);
        delete window;
    }
}

Platform::~Platform() {
    for (Window* window : this->windows) {
        delete window;
    }
    this->windows.clear();
}

}