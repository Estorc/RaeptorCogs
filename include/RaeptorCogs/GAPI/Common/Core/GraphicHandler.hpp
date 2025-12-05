/** ********************************************************************************
 * @section GAPI_Common_Core_GraphicHandler_Overview Overview
 * @file GraphicHandler.hpp
 * @brief GraphicHandler interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for graphic batch handling
 * *********************************************************************************
 * @section GAPI_Common_Core_GraphicHandler_Header Header
 * <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_GraphicHandler_Metadata Metadata
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
#include <iostream>
#include <vector>
#include <cstdint>
#include <tuple>

namespace RaeptorCogs {
    class Graphic2D;

    /**
     * @brief Batch key structure.
     * 
     * Represents a unique key for graphic batching based on rendering parameters.
     */
    struct BatchKey {
        /**
         * @brief Writing mask.
         * 
         * Indicates which buffers are written to during rendering.
         * 
         * @note Used for batching graphics with similar write operations.
         */
        int writingMask;

        /**
         * @brief Reading mask.
         * 
         * Indicates which buffers are read from during rendering.
         * 
         * @note Used for batching graphics with similar read operations.
         */
        int readingMask;

        /**
         * @brief Z-index for rendering order.
         * 
         * Determines the rendering order of graphics, with lower values rendered first.
         * 
         * @note Used for batching graphics with similar depth.
         */
        int zindex;

        /**
         * @brief Opaqueness flag.
         * 
         * Indicates whether the graphic is opaque or transparent.
         * 
         * @note Used for batching graphics with similar transparency.
         */
        bool isOpaque;

        /**
         * @brief Shader program ID.
         * 
         * Identifier for the shader program used for rendering.
         * 
         * @note Used for batching graphics using the same shader.
         */
        uint32_t programID;

        /**
         * @brief Texture ID.
         * 
         * Identifier for the texture used for rendering.
         * 
         * @note Used for batching graphics using the same texture.
         */
        uint32_t textureID;

        /**
         * @brief Less-than operator for ordering.
         * 
         * @param other The other BatchKey to compare with.
         * @return true if this BatchKey is less than the other, false otherwise.
         * 
         * @note Used for sorting BatchKeys in collections.
         */
        bool operator<(const BatchKey& other) const noexcept {
            return std::tie(writingMask, readingMask, zindex, isOpaque, programID, textureID)
                < std::tie(other.writingMask, other.readingMask, other.zindex, other.isOpaque, other.programID, other.textureID);
        }

        /**
         * @brief Greater-than operator for ordering.
         * 
         * @param other The other BatchKey to compare with.
         * @return true if this BatchKey is greater than the other, false otherwise.
         * 
         * @note Used for sorting BatchKeys in collections.
         */
        bool operator>(const BatchKey& other) const noexcept {
            return other < *this;
        }

        /**
         * @brief Not-equal operator.
         * 
         * @param other The other BatchKey to compare with.
         * @return true if this BatchKey is not equal to the other, false otherwise.
         * 
         * @note Used for comparing BatchKeys in collections.
         */
        bool operator!=(const BatchKey& other) const noexcept {
            return !(*this == other);
        }

        /**
         * @brief Equality operator.
         * 
         * @param other The other BatchKey to compare with.
         * @return true if this BatchKey is equal to the other, false otherwise.
         * 
         * @note Used for comparing BatchKeys in collections.
         */
        bool operator==(const BatchKey& other) const noexcept {
            return writingMask == other.writingMask &&
                readingMask == other.readingMask &&
                zindex      == other.zindex &&
                isOpaque    == other.isOpaque &&
                programID   == other.programID &&
                textureID   == other.textureID;
        }
    };
}


namespace RaeptorCogs::GAPI::Common {
    /**
     * @brief Graphic batch handler structure.
     * 
     * Manages batching information for a graphic during rendering.
     */
    struct GraphicBatchHandler {
        /**
         * @brief Static data cursor.
         * 
         * Indicates the position in the static instance data buffer.
         * 
         * @note Not size_t to replicate GL buffer offset limitations.
         */
        unsigned int staticDataCursor;

        /**
         * @brief Dynamic data cursor.
         * 
         * Indicates the position in the dynamic instance data buffer.
         * 
         * @note Not size_t to replicate GL buffer offset limitations.
         */
        unsigned int dynamicDataCursor;

        /**
         * @brief Dynamic data size.
         * 
         * Indicates the size of the dynamic instance data for this graphic.
         * 
         * @note Not size_t to replicate GL buffer size limitations.
         */
        unsigned int dynamicDataSize;

        /**
         * @brief Renderer key.
         * 
         * Unique key for batching based on rendering parameters.
         */
        BatchKey rendererKey;

        /**
         * @brief Pointer to the associated graphic.
         * 
         * The graphic being managed by this batch handler.
         */
        Graphic2D* graphic;

        /**
         * @brief Dirty flag.
         * 
         * Indicates whether the graphic's data has changed and needs to be re-uploaded.
         */
        bool isDirty;

        /**
         * @brief Constructor for GraphicBatchHandler.
         * 
         * @param key The BatchKey for this handler.
         * @param graphic Pointer to the associated Graphic2D.
         * 
         * @note Initializes cursors and dirty flag.
         */
        GraphicBatchHandler(const BatchKey& key, Graphic2D* graphic) : staticDataCursor(0), dynamicDataCursor(0), dynamicDataSize(0), rendererKey(key), graphic(graphic), isDirty(false) {}
    };
}

namespace std {

    /**
     * @brief Stream output operator for BatchKey.
     * 
     * @param os Output stream.
     * @param key BatchKey instance to output.
     * @return Reference to the output stream.
     * 
     * @note Outputs the BatchKey in a human-readable format.
     */
    inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::BatchKey& key) {
        os << "{ writeMask=" << key.writingMask
        << ", readMaskID=" << key.readingMask
        << ", z=" << key.zindex
        << ", opaque=" << key.isOpaque
        << ", program=" << key.programID
        << ", texture=" << key.textureID
        << " }";
        return os;
    }

    /**
     * @brief Stream output operator for GraphicBatchHandler.
     * 
     * @param os Output stream.
     * @param handler GraphicBatchHandler instance to output.
     * @return Reference to the output stream.
     * 
     * @note Outputs the GraphicBatchHandler in a human-readable format.
     */
    inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::GAPI::Common::GraphicBatchHandler& handler) {
        os << "GraphicBatchHandler { staticDataCursor=" << handler.staticDataCursor
            << ", dynamicDataCursor=" << handler.dynamicDataCursor
            << ", dynamicDataSize=" << handler.dynamicDataSize
            << ", rendererKey=" << handler.rendererKey
            << ", graphic=" << handler.graphic;
        os << " }";
        return os;
    }
}