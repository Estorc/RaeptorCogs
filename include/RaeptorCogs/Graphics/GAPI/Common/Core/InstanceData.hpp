/** ********************************************************************************
 * @section GAPI_Common_Core_InstanceData_Overview Overview
 * @file InstanceData.hpp
 * @brief InstanceData interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for graphic batch handling
 * *********************************************************************************
 * @section GAPI_Common_Core_InstanceData_Header Header
 * <RaeptorCogs/Graphics/GAPI/Common/Core/InstanceData.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_InstanceData_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <RaeptorCogs/Core/Region.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Core/GraphicHandler.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Buffer.hpp>
#include <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogsShaders/common/constants.glsl>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vector>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Struct representing the data for a single instance in a batch.
 * This includes both static data (model matrix, UV rect, etc.) and dynamic data
 * (color, etc.).
 */
struct alignas(16) StaticInstanceData {
    /** Model matrix of the instance */
    glm::mat4 model; // 64 bytes
    /** UV rectangle of the instance */
    glm::vec4 uvRect; // 16 bytes
    /** Renderer mode of the instance */
    RendererMode type; // 4 bytes
    /** Offset into the dynamic instance data buffer */
    unsigned int dataOffset; // 4 bytes
    /** Writing mask ID */
    int writingMaskID; // 4 bytes
    /** Reading mask ID */
    int readingMaskID; // 4 bytes
};
/**
 * @brief Type alias for dynamic instance data (e.g., color, smoothness).
 * This is a simple float for now, but can be expanded to a struct if needed.
 */
using DynamicInstanceData = float;
/**
 * @brief Type alias for a buffer of static instance data.
 */
using StaticInstanceDataBuffer = std::vector<StaticInstanceData>;
/**
 * @brief Type alias for a buffer of dynamic instance data.
 */
using DynamicInstanceDataBuffer = std::vector<float>;
/**
 * @brief Struct representing the complete instance data for a batch, including both
 * static and dynamic data buffers.
 */
struct InstanceData {
  private:
    /**
     * Static instance data buffer, containing data that doesn't change frequently (e.g.,
     * model matrix, UV rect).
     */
    StaticInstanceDataBuffer staticData;

    /**
     * Dynamic instance data buffer, containing data that can change frequently (e.g.,
     * color).
     */
    DynamicInstanceDataBuffer dynamicData;

  public:
    /**
     * @brief Get the static instance data buffer.
     * @return Reference to the static instance data buffer.
     */
    StaticInstanceDataBuffer &getStatic() {
      return staticData;
    }
    /**
     * @brief Get the dynamic instance data buffer.
     * @return Reference to the dynamic instance data buffer.
     */
    DynamicInstanceDataBuffer &getDynamic() {
      return dynamicData;
    }
};

/**
 * @brief Class responsible for allocating instance data for graphics batches.
 * This class manages the allocation of static and dynamic instance data for
 * graphics batches, ensuring efficient use of memory and minimizing fragmentation.
 */
class InstanceAllocator {
  private:
    /**
     * Reference to the complete instance data for a batch, which includes both static and
     * dynamic data buffers. The allocator will manage allocations within these buffers
     * for individual instances in the batch.
     */
    InstanceData &instanceData;

    /**
     * Region allocator for free static data regions. This manages the allocation and
     * deallocation of regions within the static instance data buffer.
     */
    RegionAllocator freeStaticDataRegionsAllocator;

    /**
     * Region allocator for free dynamic data regions. This manages the allocation and
     * deallocation of regions within the dynamic instance data buffer.
     */
    RegionAllocator freeDynamicDataRegionsAllocator;

    /**
     * Allocate a region in the static instance data buffer.
     * @return Offset of the allocated region in the static instance data buffer.
     */
    size_t allocateStaticData();

    /**
     * Allocate a region in the dynamic instance data buffer.
     * @param size Size of the dynamic data to allocate (in number of floats).
     * @return Offset of the allocated region in the dynamic instance data buffer.
     */
    size_t allocateDynamicData(size_t size);

    /**
     * Free a region in the static instance data buffer.
     * @param begin Start offset of the region to free.
     * @param end End offset of the region to free.
     */
    void freeStaticData(size_t begin, size_t end);

    /**
     * Free a region in the dynamic instance data buffer.
     * @param begin Start offset of the region to free.
     * @param end End offset of the region to free.
     */
    void freeDynamicData(size_t begin, size_t end);

  public:
    /**
     * @brief Constructor for the InstanceAllocator class.
     * @param instanceData Reference to the complete instance data for a batch, which
     * includes both static and dynamic data buffers. The allocator will manage
     * allocations within these buffers for individual instances in the batch.
     */
    InstanceAllocator(InstanceData &instanceData) : instanceData(instanceData) {}

    /**
     * @brief Allocate instance data for a graphics batch. This will allocate both static
     * and dynamic data for the batch, and return the offsets for each instance in the
     * batch.
     */
    void allocate(GraphicBatchHandler &batchHandler, size_t dynamicDataSize);

    /**
     * @brief Free instance data for a graphics batch. This will free both static and
     * dynamic data for the batch, based on the offsets stored in the batch handler.
     */
    void free(GraphicBatchHandler &batchHandler);

    /**
     * @brief Get a static instance data object at a given offset.
     * @param offset Offset of the static instance data to retrieve.
     * @return Reference to the static instance data object at the given offset.
     */
    StaticInstanceData &getStaticInstanceData(size_t offset);

    /**
     * @brief Get a pointer to dynamic instance data at a given offset.
     * @param offset Offset of the dynamic instance data to retrieve.
     * @return Pointer to the dynamic instance data at the given offset.
     */
    DynamicInstanceData *getDynamicInstanceData(size_t offset);
};

/**
 * @brief Class responsible for uploading instance data to the GPU. This class manages
 * the tracking of dirty regions in the instance data buffers and performs efficient
 * uploads to the GPU when necessary.
 */
class InstanceUploader {
  private:
    /**
     * Reference to the complete instance data for a batch, which includes both static and
     * dynamic data buffers. The uploader will read from these buffers to upload data
     * to the GPU.
     */
    InstanceData &instanceData;

    /**
     * Buffer for tracking dirty regions in the static instance data buffer. This allows
     * the uploader to only upload the regions of the static data that have changed,
     * improving performance.
     */
    RegionBuffer dirtyStaticDataRegions;

    /**
     * Buffer for tracking dirty regions in the dynamic instance data buffer. This allows
     * the uploader to only upload the regions of the dynamic data that have changed,
     * improving performance.
     */
    RegionBuffer dirtyDynamicDataRegions;

  public:
    /**
     * @brief Constructor for the InstanceUploader class.
     * @param instanceData Reference to the complete instance data for a batch, which
     * includes both static and dynamic data buffers. The uploader will read from these
     * buffers to upload data to the GPU.
     */
    InstanceUploader(InstanceData &instanceData) : instanceData(instanceData) {}

    /**
     * @brief Mark a region of the static instance data buffer as dirty, indicating that
     * it needs to be uploaded to the GPU.
     * @param offset Offset of the region to mark as dirty.
     * @param size Size of the region to mark as dirty (in number of StaticInstanceData
     * objects).
     */
    void markStaticDataDirty(size_t offset, size_t size);

    /**
     * @brief Mark a region of the dynamic instance data buffer as dirty, indicating that
     * it needs to be uploaded to the GPU.
     * @param offset Offset of the region to mark as dirty.
     * @param size Size of the region to mark as dirty (in number of floats).
     */
    void markDynamicDataDirty(size_t offset, size_t size);

    /**
     * @brief Upload the dirty regions of the instance data buffers to the GPU. This will
     * only upload the regions that have been marked as dirty, improving performance.
     * @param staticInstanceDataSSBO Object handler for the static instance data SSBO, to
     * which the static instance data will be uploaded.
     * @param dynamicInstanceDataSSBO Object handler for the dynamic instance data SSBO,
     * to which the dynamic instance data will be uploaded.
     */
    void
    upload(RaeptorCogs::GAPI::ObjectHandler<SSBO> *staticInstanceDataSSBO,
           RaeptorCogs::GAPI::ObjectHandler<SSBO> *dynamicInstanceDataSSBO);
};

} // namespace RaeptorCogs::GAPI::Common