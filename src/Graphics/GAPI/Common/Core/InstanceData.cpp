#include <RaeptorCogs/Graphics/GAPI/Common/Core/InstanceData.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/RendererBackend.hpp>
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
        staticInstanceDataSSBO->get()->upload(this->instanceData.getStatic().data() + begin, (end - begin) * sizeof(Common::StaticInstanceData), begin * sizeof(Common::StaticInstanceData));
    }
    for (const auto& region : this->dirtyDynamicDataRegions) {
        size_t begin = region.first;
        size_t end = region.second;
        dynamicInstanceDataSSBO->get()->bind();
        dynamicInstanceDataSSBO->get()->upload(this->instanceData.getDynamic().data() + begin, (end - begin) * sizeof(Common::DynamicInstanceData), begin * sizeof(Common::DynamicInstanceData));
    }
    this->dirtyDynamicDataRegions.clear();
    this->dirtyStaticDataRegions.clear();
}
}