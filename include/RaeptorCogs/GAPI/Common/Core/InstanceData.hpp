/** ********************************************************************************
 * @section GAPI_Common_Core_InstanceData_Overview Overview
 * @file InstanceData.hpp
 * @brief InstanceData interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for graphic batch handling
 * *********************************************************************************
 * @section GAPI_Common_Core_InstanceData_Header Header
 * <RaeptorCogs/GAPI/Common/Core/InstanceData.hpp>
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
#include <RaeptorCogs/Region.hpp>
#include <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Buffer.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>
#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace RaeptorCogs::GAPI::Common {

struct alignas(16) StaticInstanceData {
    glm::mat4 model;              // 64 bytes
    glm::vec4 uvRect;             // 16 bytes
    int type;                     // 4 bytes
    unsigned int dataOffset;               // 4 bytes
    int writingMaskID;              // 4 bytes
    int readingMaskID;               // 4 bytes
};
using DynamicInstanceData = float;
using StaticInstanceDataBuffer = std::vector<StaticInstanceData>;
using DynamicInstanceDataBuffer = std::vector<float>;


struct InstanceData {
    private:
        StaticInstanceDataBuffer staticData;
        DynamicInstanceDataBuffer dynamicData;
    public:
        StaticInstanceDataBuffer& getStatic() { return staticData; }
        DynamicInstanceDataBuffer& getDynamic() { return dynamicData; }
};


class InstanceAllocator {
    private:
        InstanceData& instanceData;
        RegionAllocator freeStaticDataRegionsAllocator;
        RegionAllocator freeDynamicDataRegionsAllocator;

        size_t allocateStaticData();
        size_t allocateDynamicData(size_t size);

        void freeStaticData(size_t begin, size_t end);
        void freeDynamicData(size_t begin, size_t end);
    
    public:
        InstanceAllocator(InstanceData& instanceData) : instanceData(instanceData) {}
        
        void allocate(GraphicBatchHandler& batchHandler, size_t dynamicDataSize);
        void free(GraphicBatchHandler& batchHandler);

        StaticInstanceData& getStaticInstanceData(size_t offset);
        DynamicInstanceData* getDynamicInstanceData(size_t offset);

};

class InstanceUploader {
    private:
        InstanceData& instanceData;
        RegionBuffer dirtyStaticDataRegions;
        RegionBuffer dirtyDynamicDataRegions;
    public:
        InstanceUploader(InstanceData& instanceData) : instanceData(instanceData) {}
        void markStaticDataDirty(size_t offset, size_t size);
        void markDynamicDataDirty(size_t offset, size_t size);
        void upload(RaeptorCogs::GAPI::ObjectHandler<SSBO>* staticInstanceDataSSBO, RaeptorCogs::GAPI::ObjectHandler<SSBO>* dynamicInstanceDataSSBO);
};

}