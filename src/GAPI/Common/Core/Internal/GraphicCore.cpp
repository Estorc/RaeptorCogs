#include <RaeptorCogs/GAPI/Common/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/GAPI/Common/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/GAPI/Common/RendererBackend.hpp>

namespace RaeptorCogs::GAPI::Common {

void GraphicCore::updateGraphicGPUData() {
    RenderPipeline& pipeline = this->getRenderer().getRenderPipeline();
    if (pipeline.getRenderList().wasReordered()) {
        pipeline.getRenderList().uploadOrderIndices();
    }
    this->getInstanceUploader().upload(&this->staticInstanceDataSSBO, &this->dynamicInstanceDataSSBO);
}

ObjectHandler<Common::SSBO>& GraphicCore::getStaticInstanceDataSSBO() {
    return this->staticInstanceDataSSBO;
}

ObjectHandler<Common::SSBO>& GraphicCore::getDynamicInstanceDataSSBO() {
    return this->dynamicInstanceDataSSBO;
}

ObjectHandler<Common::VBO>& GraphicCore::getQuadVBO() {
    return this->quadVBO;
}

ObjectHandler<Common::EBO>& GraphicCore::getQuadEBO() {
    return this->quadEBO;
}

}