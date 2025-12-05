/** ********************************************************************************
 * @section GAPI_Common_Core_RenderList_Overview Overview
 * @file RenderList.hpp
 * @brief Render list interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for render list management
 * *********************************************************************************
 * @section GAPI_Common_Core_RenderList_Header Header
 * <RaeptorCogs/GAPI/Common/Core/RenderList.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_RenderList_Metadata Metadata
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
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>
#include <RaeptorCogs/GAPI/Common/Core/InstanceData.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Buffer.hpp>
#include <RaeptorCogs/GAPI/Common/Resources/Object.hpp>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <execution>

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Order indices buffer.
 * 
 * Holds the order of indices for rendering.
 * 
 * @note Used for indirect rendering and reordering.
 */
using OrderIndicesBuffer = std::vector<unsigned int>;

/**
 * @brief Graphic batch handler structure.
 * 
 * Holds information about a graphic batch for rendering.
 * 
 * @note Used for batching graphics with similar rendering parameters.
 */
using BatchBuffer = std::vector<GAPI::Common::GraphicBatchHandler>;

/**
 * @brief Dirty handlers buffer.
 * 
 * Holds references to graphic batch handlers that need re-ordering.
 * 
 * @note Used for tracking graphics that have changed and need to be re-ordered.
 */
using DirtyHandlersBuffer = std::vector<std::reference_wrapper<GraphicBatchHandler>>;

/**
 * @brief Render list flags enumeration.
 * 
 * Defines flags for render list state management.
 * 
 * @note Used for tracking the state of the render list.
 */
enum class RenderListFlags : uint32_t {
    /** No flags set. */
    NONE = 0,
    /** Needs reordering. */
    NEEDS_REORDER = 1 << 0,
    /** Has been reordered. */
    REORDERED = 1 << 1,
    /** SSBO created. */
    SSBO_CREATED = 1 << 2
};

}

namespace RaeptorCogs {

class Graphic2D;

}

namespace RaeptorCogs::GAPI::Common {

/**
 * @brief Indirect iterator for render list.
 * 
 * Provides an iterator to traverse graphic batch handlers in a render list indirectly.
 * 
 * @note Used for iterating over graphics in a render list based on order indices.
 */
class IndirectIterator {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Reference to the batch buffer.
         * 
         * Holds the batch handlers for rendering.
         */
        BatchBuffer& batch_;

        /**
         * @brief Reference to the order indices buffer.
         * 
         * Holds the order of indices for rendering.
         */
        const OrderIndicesBuffer& order_;

        /**
         * @brief Current index in the order indices buffer.
         * 
         * Indicates the current position in the iteration.
         */
        size_t index_;
        
    public:
        /** Iterator category for indirect traversal of graphic batch handlers */
        using iterator_category  = std::forward_iterator_tag;
        /** Difference type for iterator */
        using difference_type   = std::ptrdiff_t;
        /** Value type for iterator */
        using value_type        = GraphicBatchHandler;
        /** Pointer type for iterator */
        using pointer           = GraphicBatchHandler*;
        /** Reference type for iterator */
        using reference         = GraphicBatchHandler&;

        /**
         * @brief Constructor for IndirectIterator.
         * 
         * @param batch Reference to the batch buffer.
         * @param order Reference to the order indices buffer.
         * @param index Initial index for the iterator.
         */
        IndirectIterator(BatchBuffer& batch, const OrderIndicesBuffer& order, size_t index)
            : batch_(batch), order_(order), index_(index) {}

        /**
         * @brief Ordered item structure.
         * 
         * Holds the index and reference to a graphic batch handler.
         * 
         * @note Used for dereferencing the iterator.
         */
        struct OrderedItem {
            /** Index of the ordered item */
            size_t index;
            /** Reference to the graphic batch handler */
            reference value;
        };

        /**
         * @brief Dereference operator for IndirectIterator.
         * 
         * @return OrderedItem containing the index and reference to the graphic batch handler.
         */
        OrderedItem operator*() const {
            return { index_, batch_[order_[index_]] };
        }
        /**
         * @brief Arrow operator for IndirectIterator.
         * 
         * @return Pointer to the graphic batch handler.
         */
        pointer operator->() const { return &batch_[order_[index_]]; }

        /**
         * @brief Pre-increment operator for IndirectIterator.
         * 
         * @return Reference to the incremented iterator.
         */
        IndirectIterator& operator++() { ++index_; return *this; }

        /**
         * @brief Post-increment operator for IndirectIterator.
         * 
         * @return Copy of the iterator before increment.
         */
        IndirectIterator operator++(int) { IndirectIterator tmp = *this; ++(*this); return tmp; }

        /**
         * @brief Equality operator for IndirectIterator.
         * 
         * @param other The other IndirectIterator to compare with.
         * @return true if both iterators are equal, false otherwise.
         * 
         * @note Compares the current indices of both iterators.
         */
        bool operator==(const IndirectIterator& other) const { return index_ == other.index_; }

        /**
         * @brief Not-equal operator for IndirectIterator.
         * 
         * @param other The other IndirectIterator to compare with.
         * @return true if both iterators are not equal, false otherwise.
         * 
         * @note Compares the current indices of both iterators.
         */
        bool operator!=(const IndirectIterator& other) const { return !(*this == other); }
};

/**
 * @brief Render list class.
 * 
 * Manages a list of graphic batch handlers for rendering.
 * 
 * @note Used for organizing and rendering graphics in batches.
 */
class RenderList {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Reference to the batch buffer.
         * 
         * Holds the batch handlers for rendering.
         * 
         * @note Shared with the renderer backend.
         */
        BatchBuffer& batch;

        /**
         * @brief Order indices buffer.
         * 
         * Holds the order of indices for rendering.
         * 
         * @note Used for indirect rendering and reordering.
         */
        OrderIndicesBuffer orderIndices;

        /**
         * @brief Dirty handlers buffer.
         * 
         * Holds references to graphic batch handlers that need re-ordering.
         */
        DirtyHandlersBuffer dirtyHandlers;

        /**
         * @brief Render list flags.
         * 
         * Holds the state flags for the render list.
         */
        RenderListFlags flags;

        /**
         * @brief SSBO for index indirection.
         * 
         * Holds the shader storage buffer object for index indirection.
         */
        RaeptorCogs::GAPI::ObjectHandler<Common::SSBO> indexIndirectionSSBO;

        // ============================================================================
        //                             PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Do a binary search reorder of the render list.
         * 
         * @param handler Reference to the GraphicBatchHandler that triggered the reorder.
         * 
         * @note Uses binary search to find the correct position for the handler.
         */
        void binarySearchReorder(const GraphicBatchHandler& handler);

        /**
         * @brief Radix reorder of the render list.
         * 
         * @note Uses radix sort algorithm for reordering.
         */
        void radixReorder();

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for RenderList.
         * 
         * @param batch Reference to the batch buffer.
         * 
         * @note Initializes the render list with the given batch buffer.
         */
        RenderList(BatchBuffer& batch);

        /**
         * @brief Get the graphic batch handler at the specified index.
         * 
         * @param index Index of the graphic batch handler to retrieve.
         * @return Reference to the GraphicBatchHandler.
         * 
         * @note Used for accessing batch handlers directly.
         */
        GraphicBatchHandler& getHandler(size_t index);

        /**
         * @brief Get the graphic batch handler indirectly at the specified index.
         * 
         * @param index Index of the graphic batch handler to retrieve.
         * @return Reference to the GraphicBatchHandler.
         * 
         * @note Used for accessing batch handlers based on order indices.
         */
        GraphicBatchHandler& getIndirectHandler(size_t index);

        /**
         * @brief Create a new graphic batch handler.
         * 
         * @param key BatchKey for the new handler.
         * @param graphic Pointer to the associated Graphic2D.
         * @param instanceAllocator Reference to the InstanceAllocator.
         * 
         * @return Reference to the created GraphicBatchHandler.
         */
        GraphicBatchHandler& createHandler(BatchKey key, Graphic2D* graphic, InstanceAllocator& instanceAllocator);

        /**
         * @brief Check if the render list is empty.
         * 
         * @return true if the render list is empty, false otherwise.
         */
        bool empty() const;

        /**
         * @brief Clear the render list.
         * 
         * @note Removes all graphic batch handlers from the render list.
         */
        void clear();

        /**
         * @brief Erase a graphic batch handler from the render list.
         * 
         * @param handler Reference to the GraphicBatchHandler to erase.
         * @param instanceAllocator Reference to the InstanceAllocator.
         */
        void erase(GraphicBatchHandler& handler, InstanceAllocator& instanceAllocator);

        /**
         * @brief Bind the index indirection SSBO.
         */
        void bind();

        /**
         * @brief Check if reordering is needed.
         * 
         * @return true if reordering is needed, false otherwise.
         */
        bool needsReorder() const;

        /**
         * @brief Check if the render list was reordered.
         * 
         * @return true if the render list was reordered, false otherwise.
         */
        bool wasReordered() const;

        /**
         * @brief Reorder the render list.
         * 
         * @note Chooses the appropriate reordering method based on the number of dirty handlers.
         */
        void reorder();

        /**
         * @brief Mark a graphic batch handler as dirty.
         * 
         * @param handler Reference to the GraphicBatchHandler to mark as dirty.
         * 
         * @note Adds the handler to the dirty handlers buffer.
         */
        void markDirty(GraphicBatchHandler& handler);

        /**
         * @brief Upload the order indices to the SSBO.
         */
        void uploadOrderIndices();

        /**
         * @brief Get the size of the render list.
         * 
         * @return The number of graphic batch handlers in the render list.
         */
        size_t size() const { return orderIndices.size(); }

        /**
         * @brief Begin iterator for the render list.
         * 
         * @return IndirectIterator to the beginning of the render list.
         */
        IndirectIterator begin() { return IndirectIterator(batch, orderIndices, 0); }

        /**
         * @brief End iterator for the render list.
         * 
         * @return IndirectIterator to the end of the render list.
         */
        IndirectIterator end()   { return IndirectIterator(batch, orderIndices, orderIndices.size()); }
};

/**
 * @brief Render list buffer type.
 * 
 * Maps integer keys to RenderList instances.
 * 
 * @note Used for managing multiple render lists.
 */
using RenderListBuffer = std::unordered_map<int, RenderList>;
}