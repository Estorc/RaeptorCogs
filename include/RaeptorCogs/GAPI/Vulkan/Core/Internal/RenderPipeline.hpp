/** ********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Overview Overview
 * @file RenderPipeline.hpp
 * @brief Render pipeline interface.
 * @details
 * Typical use cases:
 * - Providing Vulkan-specific implementations for the render pipeline interface
 * *********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Core/Internal/RenderPipeline.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_RenderPipeline_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/RenderPipeline.hpp>
#include <vulkan/vulkan.h>

namespace RaeptorCogs::GAPI::Vulkan {

class RenderPipeline : public Common::RenderPipeline {
    private:

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::beginFrame()
         */
        void beginFrame() override;

        /**
         * @see RaeptorCogs::GAPI::Common::RendererBackend::endFrame()
         */
        void endFrame() override;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for RenderPipeline.
         */
        RenderPipeline(Common::RendererBackend& renderer) : Common::RenderPipeline(renderer) {}

        /**
         * @brief Destructor for RenderPipeline.
         */
        ~RenderPipeline() = default;

};

} // namespace RaeptorCogs::GAPI::Vulkan