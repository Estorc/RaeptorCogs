/** ********************************************************************************
 * @section GAPI_Common_RenderPipeline_Overview Overview
 * @file RenderPipeline.hpp
 * @brief Render pipeline interface.
 * @details
 * Typical use cases:
 * - Providing an interface for render pipeline implementations
 * *********************************************************************************
 * @section GAPI_Common_RenderPipeline_Header Header
 * <RaeptorCogs/GAPI/Common/Core/Internal/RenderPipeline.hpp>
 ***********************************************************************************
 * @section GAPI_Common_RenderPipeline_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/FrameData.hpp>
#include <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>
#include <RaeptorCogs/GAPI/Common/Core/RenderList.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Shader.hpp>
#include <functional>


namespace RaeptorCogs {
    class Component;
    class Window;

    void MainLoop(std::function<void(Window&)> updateFunction, Window &window);
}

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Component buffer type.
 * 
 * Holds pointers to components used in rendering.
 */
using ComponentBuffer = std::vector<Component*>;

/**
 * @brief Private render list identifiers.
 * 
 * Defines reserved identifiers for internal render lists.
 */
enum class PrivateRenderListID : int {
    /** Reserved for internal draw calls */
    DRAW = std::numeric_limits<int>::max(),
};

class RendererBackend;
/**
 * @brief Render pipeline interface.
 * 
 * Provides an interface for render pipeline implementations.
 */
class RenderPipeline {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Reference to the renderer backend.
         * 
         * Holds a reference to the renderer backend singleton.
         */
        RendererBackend& renderer;

        /**
         * @brief Batch buffer.
         * 
         * Holds batch handlers for rendering.
         */
        BatchBuffer batch;

        /**
         * @brief Component buffer.
         * 
         * Holds component data for rendering.
         */
        ComponentBuffer componentBuffer;

        /**
         * @brief Render list buffer.
         * 
         * Holds render lists indexed by their IDs.
         */
        RenderListBuffer renderLists;

        /**
         * @brief Frame data.
         * 
         * Holds data relevant to the current frame.
         */
        FrameData frameData;

        /**
         * @brief Current batch index.
         * 
         * Indicates the active batch for rendering.
         */
        int currentBatchIndex = 0;

        // ============================================================================
        //                             PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Begin frame operations.
         * 
         * Called at the start of each frame.
         */
        virtual void beginFrame() = 0;

        /**
         * @brief End frame operations.
         * 
         * Called at the end of each frame.
         */
        virtual void endFrame() = 0;

        friend void RaeptorCogs::MainLoop(const std::function<void(Window&)> updateFunction, Window &window);

    protected:

        // ============================================================================
        //                             PROTECTED METHODS
        // ============================================================================

        /**
         * @brief Get the renderer pointer.
         * 
         * @return Pointer to the renderer backend singleton.
         */
        RendererBackend& getRenderer() const {
            return this->renderer;
        }

        /**
         * @brief Get the render lists.
         * 
         * @return Reference to the render list buffer.
         */
        RenderListBuffer& getRenderLists() { return this->renderLists; }

        /**
         * @brief Use the mask render list.
         * 
         * Sets the current render list to the mask render list.
         */
        void useMaskRenderList();

        /**
         * @brief Use the normal render list.
         * 
         * Sets the current render list to the normal render list.
         */
        void useNormalRenderList();

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor.
         * 
         * @param renderer Reference to the renderer backend singleton.
         */
        RenderPipeline(RendererBackend& renderer) : renderer(renderer), renderLists{{0, RenderList(this->batch)}} {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~RenderPipeline() = default;
    
        /**
         * @brief Check if two batches are compatible for merging.
         * 
         * @param a Pointer to the first GraphicBatchHandler.
         * @param b Pointer to the second GraphicBatchHandler.
         * @return True if the batches are compatible, false otherwise.
         * 
         * @note Batches are compatible if they share the same rendering parameters.
         */
        bool compatibleBatches(GraphicBatchHandler* a, GraphicBatchHandler* b);

        /**
         * @brief Begin a new batch for rendering.
         * 
         * @param x X coordinate of the batch viewport.
         * @param y Y coordinate of the batch viewport.
         * @param width Width of the batch viewport.
         * @param height Height of the batch viewport.
         * @param shader Shader to be used for the batch.
         * 
         * @note Sets up the rendering state for the new batch.
         */
        void beginBatch(int x, int y, int width, int height, ObjectHandler<Shader>& shader);

        /**
         * @brief Process the current batch for rendering.
         * 
         * @param postDrawCallback Optional callback to be executed after drawing.
         * 
         * @code{.cpp}
         * RaeptorCogs::Renderer().processBatch([]() {
         *    // Custom operations after drawing
         * });
         * @endcode
         * @note Uploads instance data and issues draw calls for the batch.
         */
        void processBatch(std::function<void()> postDrawCallback = nullptr);

        /**
         * @brief Draw a batch of instances.
         * 
         * @param firstHandler Pointer to the first GraphicBatchHandler in the batch.
         * @param instanceOffset Offset of the first instance to draw.
         * @param instanceCount Number of instances to draw.
         * @param postDrawCallback Optional callback to be executed after drawing.
         * 
         * @note Issues the draw call for the specified batch of instances.
         */
        void drawBatch(GraphicBatchHandler* firstHandler, size_t instanceOffset, size_t instanceCount, std::function<void()> postDrawCallback = nullptr);
        
        /**
         * @brief Flush the current batch.
         * 
         * Completes the current batch and prepares for the next one.
         */
        void flushBatch();

        /**
         * @see Renderer::setRenderListID
         */
        void setRenderListID(int index);

        /**
         * @brief Get the current render list.
         * 
         * @return Reference to the current render list.
         */
        RenderList& getRenderList();

        /**
         * @brief Get the mask render list of the current render list.
         * 
         * @return Reference to the mask render list.
         */
        RenderList& getMaskRenderList();

        /**
         * @brief Clear the render lists.
         * 
         * @note Removes all graphics from all render lists.
         * @warning Use with caution as this will clear all pending graphics to be rendered.
         */
        void clearRenderLists();

        /**
         * @brief Get the component buffer.
         * 
         * @return Reference to the component buffer.
         */
        ComponentBuffer &getComponentBuffer() { return this->componentBuffer; }

        /**
         * @brief Get the batch handler at the specified index.
         * 
         * @param index Index of the batch handler.
         * @return Reference to the GraphicBatchHandler.
         * 
         * @note Used for accessing batch handlers directly.
         */
        GraphicBatchHandler &getBatchHandlerAt(size_t index);

        /**
         * @brief Get the frame data.
         * 
         * @return Reference to the FrameData structure.
         */
        FrameData& getFrameData() { return this->frameData; }

};

} // namespace RaeptorCogs::GAPI::Common