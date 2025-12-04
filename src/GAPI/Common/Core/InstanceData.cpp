#include <RaeptorCogs/GAPI/Common/Core/InstanceData.hpp>
#include <RaeptorCogs/GAPI/Common/RendererBackend.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs::GAPI::Common {

size_t InstanceAllocator::allocateStaticData() {
    size_t offset = freeStaticDataRegionsAllocator.allocate(1);
    if (offset == SIZE_MAX) {
        offset = instanceData.getStatic().size();
        instanceData.getStatic().resize(offset + 1);
    }
    return offset;
}

size_t InstanceAllocator::allocateDynamicData(size_t size) {
    size_t offset = freeDynamicDataRegionsAllocator.allocate(size);
    if (offset == SIZE_MAX) {
        offset = instanceData.getDynamic().size();
        instanceData.getDynamic().resize(offset + size);
    }
    return offset;
}

void InstanceAllocator::allocate(GraphicBatchHandler& batchHandler, size_t dynamicDataSize) {
    batchHandler.staticDataCursor = static_cast<unsigned int>(this->allocateStaticData());
    batchHandler.dynamicDataSize = static_cast<unsigned int>(dynamicDataSize);
    batchHandler.dynamicDataCursor = static_cast<unsigned int>(this->allocateDynamicData(dynamicDataSize));
}

void InstanceAllocator::freeStaticData(size_t begin, size_t end) {
    freeStaticDataRegionsAllocator.free(begin, end);
}

void InstanceAllocator::freeDynamicData(size_t begin, size_t end) {
    freeDynamicDataRegionsAllocator.free(begin, end);
}

void InstanceAllocator::free(GraphicBatchHandler& batchHandler) {
    this->freeStaticData(batchHandler.staticDataCursor, batchHandler.staticDataCursor + 1);
    this->freeDynamicData(batchHandler.dynamicDataCursor, batchHandler.dynamicDataCursor + batchHandler.dynamicDataSize);
}

StaticInstanceData& InstanceAllocator::getStaticInstanceData(size_t offset) {
    return instanceData.getStatic()[offset];
}

DynamicInstanceData* InstanceAllocator::getDynamicInstanceData(size_t offset) {
    return &instanceData.getDynamic()[offset];
}


void InstanceUploader::markDynamicDataDirty(size_t offset, size_t size) {
    dirtyDynamicDataRegions.push(offset, offset + size);
}

void InstanceUploader::markStaticDataDirty(size_t offset, size_t size) {
    dirtyStaticDataRegions.push(offset, offset + size);
}

void InstanceUploader::upload(ObjectHandler<SSBO>* staticInstanceDataSSBO, ObjectHandler<SSBO>* dynamicInstanceDataSSBO) {
    for (const auto& region : this->dirtyStaticDataRegions) {
        size_t begin = region.first;
        size_t end = region.second;
        staticInstanceDataSSBO->get()->bind();
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(begin * sizeof(Common::StaticInstanceData)), static_cast<GLsizeiptr>((end - begin) * sizeof(Common::StaticInstanceData)), this->instanceData.getStatic().data() + begin);
    }
    for (const auto& region : this->dirtyDynamicDataRegions) {
        size_t begin = region.first;
        size_t end = region.second;
        #ifndef __EMSCRIPTEN__
        dynamicInstanceDataSSBO->get()->bind();
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(begin * sizeof(Common::DynamicInstanceData)), static_cast<GLsizeiptr>((end - begin) * sizeof(Common::DynamicInstanceData)), this->instanceData.getDynamic().data() + begin);
        #else
        glActiveTexture(GL_TEXTURE0 + this->getMaxTextureUnits() - 1);
        this->iDataTex->bind();
        size_t dataOffset = begin - begin % 4; // Align to 4 floats
        size_t totalFloat = this->instanceData.getDynamic().size(); // float count
        size_t texWidth = IDATATEX_WIDTH;
        size_t texHeight = ((totalFloat + 3) / 4 + texWidth - 1) / texWidth;
        size_t yOffset = (begin * sizeof(Common::DynamicInstanceData)) / (texWidth * 4 * sizeof(float));
        size_t xOffset = ((begin * sizeof(Common::DynamicInstanceData)) / sizeof(float) % (texWidth * 4)) / 4;
        size_t subWidth = std::ceil((end - begin) * sizeof(Common::DynamicInstanceData) / (4.0 * sizeof(float))) + 1;

        if (subWidth + xOffset > texWidth) {
            // Split the update into two parts if it exceeds the texture width
            size_t firstPartWidth = texWidth - xOffset;
            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, firstPartWidth, 1, GL_RGBA, GL_FLOAT, this->dynamicInstanceData.data() + dataOffset);
            size_t secondPartWidth = subWidth - firstPartWidth;
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, yOffset + 1, secondPartWidth, 1, GL_RGBA, GL_FLOAT, this->dynamicInstanceData.data() + dataOffset + firstPartWidth * 4);
        } else {
            // Single update if it fits within the texture width
            glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, subWidth, 1, GL_RGBA, GL_FLOAT, this->dynamicInstanceData.data() + dataOffset);
        }
        #endif
    }
    this->dirtyDynamicDataRegions.clear();
    this->dirtyStaticDataRegions.clear();
}
}