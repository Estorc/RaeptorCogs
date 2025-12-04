#include <RaeptorCogs/Renderer.hpp>

#include <RaeptorCogs/GAPI/GL/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>

#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Component.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Measure.hpp>
#include <iostream>
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <GLFW/glfw3.h>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


namespace RaeptorCogs::Singletons {

void Renderer::initialize(GraphicsBackend backend) {

    RaeptorCogs::Platform().setRenderer(this);
    this->setBackend(backend);
    this->setRenderListID(0);

}

void Renderer::setBackend(GraphicsBackend backend) {
    #ifdef EMSCRIPTEN
    backend = GraphicsBackend::GL;
    #endif
    if (this->activeBackend) {
        delete this->activeBackend;
        this->activeBackend = nullptr;
    }
    switch (backend) {
        case GraphicsBackend::GL:
            this->activeBackend = new GAPI::GL::RendererBackend();
            break;
        case GraphicsBackend::Vulkan:
            this->activeBackend = new GAPI::Vulkan::RendererBackend();
            break;
        default:
            break;
    }
    if (this->activeBackend) {
        this->activeBackend->setPlatform(&RaeptorCogs::Platform());
        this->activeBackend->initialize();
    }
    std::cout << "Renderer backend initialized." << std::endl;
}

Renderer::~Renderer() {
    if (this->activeBackend)
        delete this->activeBackend;
    glfwTerminate();
}


void Renderer::setRenderListID(int index) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    backend.getRenderPipeline().setRenderListID(index);
}


void Renderer::add(Graphic2D &graphic) {
    GAPI::Common::RendererBackend& backend = this->getBackend();

    graphic.setRenderer(this);
    if (graphic.hasFlag(GraphicFlags::NO_BATCHING)) {
        return;
    }

    BatchKey key = graphic.buildRendererKey();
    GAPI::Common::RenderList &renderList = key.writingMask ? backend.getRenderPipeline().getMaskRenderList() : backend.getRenderPipeline().getRenderList();
    GAPI::Common::GraphicBatchHandler &batchHandler = renderList.createHandler(key, &graphic, backend.getGraphicCore().getInstanceAllocator());

    backend.getGraphicCore().getInstanceUploader().markDynamicDataDirty(batchHandler.dynamicDataCursor, batchHandler.dynamicDataSize);
    backend.getGraphicCore().getInstanceUploader().markStaticDataDirty(batchHandler.staticDataCursor, 1);
}

void Renderer::remove(Graphic2D &graphic) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    if (RaeptorCogs::IsExiting()) {
        return;
    }
    while (graphic.getRenderLists().size() > 0) {
        GAPI::Common::RenderList* renderList = graphic.getRenderLists().back();
        for (auto [_, handler] : *renderList) {
            if (handler.graphic == &graphic) {
                renderList->erase(handler, backend.getGraphicCore().getInstanceAllocator());

                break;
            }
        }
    }
}

void Renderer::add(Component &component) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    backend.getRenderPipeline().getComponentBuffer().push_back(&component);
    std::cout << "Adding component to renderer." << std::endl;
    component.setRenderer(this);
}

void Renderer::remove(Component &component) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    auto& componentBuffer = backend.getRenderPipeline().getComponentBuffer();
    componentBuffer.erase(std::remove(componentBuffer.begin(), componentBuffer.end(), &component), componentBuffer.end());
    if (RaeptorCogs::IsExiting()) {
        return;
    }
    std::cout << "Removing component from renderer." << std::endl;
    component.setRenderer(nullptr);
}

void Renderer::render(Window& window, int width, int height) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    if (width == 0 || height == 0) {
        width = static_cast<int>(window.getWidth());
        height = static_cast<int>(window.getHeight());
    }

    backend.render(&window, 0, 0, width, height);
}

void Renderer::render(Texture& texture, int width, int height) {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    if (width == 0 || height == 0) {
        width = static_cast<int>(texture->getWidth());
        height = static_cast<int>(texture->getHeight());
    }
    int x = static_cast<int>(texture->getX());
    int y = static_cast<int>(texture->getY());
    backend.render(texture, x, y, width, height);
}


bool Renderer::isInitialized() const {
    if (!this->activeBackend) {
        return false;
    }
    return this->activeBackend->isInitialized();
}

GAPI::Common::RendererBackend& Renderer::getBackend() {
    return *this->activeBackend;
}

void Renderer::CreateImGuiFrame() {
    GAPI::Common::RendererBackend& backend = this->getBackend();
    backend.getImGuiModule().NewImGuiFrame();
}

};