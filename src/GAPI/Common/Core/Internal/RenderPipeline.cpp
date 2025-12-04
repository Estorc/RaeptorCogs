#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Common/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/Common/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/Common/Ressources/Shader.hpp>
#include <RaeptorCogs/Graphic.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <RaeptorCogs/Camera.hpp>

namespace RaeptorCogs::GAPI::Common {

GraphicBatchHandler &RenderPipeline::getBatchHandlerAt(size_t index) {
    return this->getRenderList().getHandler(index);
}

void RenderPipeline::setRenderListID(int index) {
    if (index < 0 || index == static_cast<int>(PrivateRenderListID::DRAW)) {
        throw std::runtime_error("The Render List " + std::to_string(index) + " is reserved for internal use.");
    }
    currentBatchIndex = index;
    int maskID = -index - 1;
    renderLists.try_emplace(index, RenderList(this->batch));
    renderLists.try_emplace(maskID, RenderList(this->batch));
}

RenderList& RenderPipeline::getRenderList() {
    return this->renderLists.at(currentBatchIndex);
}

RenderList& RenderPipeline::getMaskRenderList() {
    return this->renderLists.at(-currentBatchIndex - 1);
}

void RenderPipeline::useMaskRenderList() {
    if (currentBatchIndex >= 0) currentBatchIndex = -currentBatchIndex - 1;
}

void RenderPipeline::useNormalRenderList() {
    if (currentBatchIndex < 0) currentBatchIndex = -currentBatchIndex - 1;
}

void RenderPipeline::clearRenderLists() {
    for (auto& pair : this->renderLists) {
        pair.second.clear();
    }
    this->renderLists.clear();
}

bool RenderPipeline::compatibleBatches(Common::GraphicBatchHandler* a, Common::GraphicBatchHandler* b) {
    return (a->rendererKey.textureID == b->rendererKey.textureID) &&
           ((a->rendererKey.writingMask && a->rendererKey.readingMask == b->rendererKey.readingMask) ||
            (a->rendererKey.writingMask == 0 && b->rendererKey.writingMask == 0));
}

void RenderPipeline::beginBatch(int x, int y, int width, int height, ObjectHandler<Common::Shader>& shader) {
    FrameData& frameData = this->getFrameData();

    GraphicCore &graphicCore = this->getRenderer().getGraphicCore();
    this->getRenderList().bind();
    this->getRenderer().getGraphicCore().setViewport(x, y, width, height);
    this->getRenderer().getGraphicCore().clearSwapchainBuffers();

    this->getRenderer().getGraphicCore().useBlend();


    shader->bind();
    graphicCore.getQuadEBO()->bind();

    this->getRenderer().getGraphicCore().bindMaskTexture();

    frameData.width = static_cast<double>(width);
    frameData.height = static_cast<double>(height);
    frameData.activeShader = shader;
    frameData.viewMatrix = glm::mat4(1.0f);
    frameData.projectionMatrix = glm::mat4(1.0f);
    frameData.time = static_cast<float>(RaeptorCogs::Time().getTime());

    for (auto& component : componentBuffer) {
        component->update(*this);
    }

    if (frameData.projectionMatrix == glm::mat4(1.0f)) {
        frameData.projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1000.0f, 1000.0f);
    }
    if (height < 0) {
        frameData.projectionMatrix = glm::scale(frameData.projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f));
    }

    frameData.upload();
    this->getRenderer().getGraphicCore().setTextureUniform(shader);
    this->getRenderer().getGraphicCore().setMaskTextureUniform(shader);
    #ifdef __EMSCRIPTEN__
    this->getRenderer().getGraphicCore().setSSBOTextureUniform(shader);
    #endif
}

void RenderPipeline::drawBatch(Common::GraphicBatchHandler* firstHandler, size_t instanceOffset, size_t instanceCount, std::function<void()> postDrawCallback) {

    // Bind texture
    this->getRenderer().getGraphicCore().bindGraphicTexture(*firstHandler->graphic);

    
    if (firstHandler->graphic->getWritingMaskID()) {
        this->getRenderer().getGraphicCore().enableStencilGuarding();
    }

    // Draw the quad
    unsigned int baseInstance = static_cast<unsigned int>(instanceOffset);
    this->getRenderer().getGraphicCore().drawElementsInstancedBaseVertexBaseInstance(6, instanceCount, 0, 0, baseInstance);
    if (postDrawCallback) postDrawCallback();
}

void RenderPipeline::processBatch(std::function<void()> postDrawCallback) {
    GraphicCore& graphicCore = this->getRenderer().getGraphicCore();
    Common::GraphicBatchHandler* firstHandler = nullptr;
    size_t instanceOffset = 0;
    bool textureIsDirty = false;

    auto& renderList = this->getRenderList();
    if (renderList.empty()) return;
    if (renderList.needsReorder()) renderList.reorder();
    graphicCore.updateGraphicGPUData();
    for (auto [index, handler] : renderList) {

        if (firstHandler != nullptr && !this->compatibleBatches(firstHandler, &handler)) {
            this->drawBatch(firstHandler, instanceOffset, index - instanceOffset, postDrawCallback);
            firstHandler = nullptr;
            instanceOffset = index;
        }

        if (firstHandler == nullptr) {
            firstHandler = &handler;
            if (handler.graphic->getTexture()) {
                textureIsDirty = handler.graphic->getTexture()->needsRebuild();
            }
        }
        
        if ((textureIsDirty || handler.isDirty) && handler.graphic->computeInstanceData(graphicCore.getInstanceAllocator(), textureIsDirty ? ComputeInstanceDataMode::REBUILD_TEXTURE : ComputeInstanceDataMode::NONE)) {            
            graphicCore.getInstanceUploader().markDynamicDataDirty(handler.dynamicDataCursor, handler.dynamicDataSize);
            graphicCore.getInstanceUploader().markStaticDataDirty(handler.staticDataCursor, 1);
            graphicCore.updateGraphicGPUData();
        }
    }
    if (firstHandler != nullptr && instanceOffset < renderList.size()) {
        this->drawBatch(firstHandler, instanceOffset, renderList.size() - instanceOffset, postDrawCallback);
    }
}

void RenderPipeline::flushBatch() {
    //
}

}