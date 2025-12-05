/** ********************************************************************************
 * @section Region_Overview Overview
 * @file Region.hpp
 * @brief High-level region management utilities.
 * @details
 * Typical use cases:
 * - Managing memory regions for allocation and deallocation.
 * - Providing region merging and splitting functionalities.
 * *********************************************************************************
 * @section Region_Header Header
 * <RaeptorCogs/Region.hpp>
 ***********************************************************************************
 * @section Region_Metadata Metadata
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
#include <stdexcept>
#include <algorithm>
#include <cstdint>

namespace RaeptorCogs { 

/**
 * @brief Region type representing an interval [begin, end).
 * 
 * Used by RegionBuffer and RegionAllocator for managing memory regions.
 * 
 * @code{.cpp}
 * RaeptorCogs::Region region = std::make_pair(0, 1024); // begin = 0, end = 1024
 * @endcode
 */
using Region = std::pair<size_t, size_t>;

/**
 * @brief RegionBuffer class.
 * 
 * Manages a collection of memory regions, allowing for merging, splitting, and retrieval.
 * 
 * @code{.cpp}
 * RaeptorCogs::RegionBuffer buffer;
 * buffer.push(0, 512);
 * buffer.push(1024, 2048);
 * @endcode
 */
class RegionBuffer { 
    private: 

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Underlying storage for the regions.
         * 
         * Stores a vector of Region pairs.
         */
        std::vector<Region> regions;
    public: 

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for RegionBuffer.
         */
        RegionBuffer() = default;

        /**
         * @brief Destructor for RegionBuffer.
         */
        ~RegionBuffer() = default;

        /**
         * @brief Push a new region into the buffer.
         * 
         * @param begin The beginning of the region.
         * @param end The end of the region.
         * 
         * @note Merges overlapping or adjacent regions automatically.
         */
        void push(size_t begin, size_t end) {
            if (begin > end)
                std::swap(begin, end);

            Region newRegion(begin, end);

            // Insert position based on begin
            auto it = std::lower_bound(regions.begin(), regions.end(), newRegion,
                [](const auto& a, const auto& b) {
                    return a.first < b.first;
                }
            );

            // Insert
            it = regions.insert(it, newRegion);

            // Merge with next
            auto mergeNext = it;
            ++mergeNext;
            while (mergeNext != regions.end() && mergeNext->first <= it->second + 1) {
                it->second = std::max(it->second, mergeNext->second);
                mergeNext = regions.erase(mergeNext);
            }

            // Merge with previous
            if (it != regions.begin()) {
                auto prev = it - 1;
                if (prev->second + 1 >= it->first) {
                    prev->second = std::max(prev->second, it->second);
                    regions.erase(it);
                }
            }
        } 

        /**
         * @brief Clear all regions from the buffer.
         * 
         * @note Resets the RegionBuffer to an empty state.
         */
        void clear() { 
            regions.clear(); 
        } 

        /**
         * @brief Erase a region from the buffer.
         * 
         * @param begin The beginning of the region to erase.
         * @param end The end of the region to erase.
         * 
         * @note Splits regions if necessary.
         */
        void erase(size_t begin, size_t end) {
            if (begin > end)
                std::swap(begin, end);

            Region eraseRegion(begin, end);

            auto it = regions.begin();
            while (it != regions.end()) {
                if (it->second <= eraseRegion.first) {
                    ++it;
                    continue;
                }
                if (it->first >= eraseRegion.second) {
                    break;
                }

                if (it->first < eraseRegion.first && it->second > eraseRegion.second) {
                    // Split region
                    Region newRegion(eraseRegion.second, it->second);
                    it->second = eraseRegion.first;
                    regions.insert(it + 1, newRegion);
                    break;
                } else if (it->first < eraseRegion.first) {
                    // Adjust end
                    it->second = eraseRegion.first;
                    ++it;
                } else if (it->second > eraseRegion.second) {
                    // Adjust start
                    it->first = eraseRegion.second;
                    break;
                } else {
                    // Remove region
                    it = regions.erase(it);
                }
            }
        }

        /**
         * @brief Get the last region in the buffer.
         * 
         * @return Region The last region.
         * 
         * @throws std::runtime_error if no regions are available.
         */
        Region get() { 
            if (regions.empty()) {
                throw std::runtime_error("No regions available");
            }
            return regions.back();
        }
        
        /**
         * @brief Get the number of regions in the buffer.
         * 
         * @return size_t The number of regions.
         */
        size_t size() const { 
            return regions.size(); 
        } 

        /**
         * @brief Check if the buffer is empty.
         * 
         * @return true if empty, false otherwise.
         */
        bool empty() const { 
            return regions.empty(); 
        }

        /**
         * @brief Iterator support for RegionBuffer.
         */
        auto begin() { 
            return regions.begin(); 
        } 
        
        /**
         * @brief Iterator support for RegionBuffer.
         */
        auto end() { 
            return regions.end(); 
        } 

        /**
         * @brief Const iterator support for RegionBuffer.
         */
        auto begin() const { 
            return regions.begin(); 
        } 
        
        /**
         * @brief Const iterator support for RegionBuffer.
         */
        auto end() const { 
            return regions.end(); 
        } 
};


/**
 * @brief RegionAllocator class.
 * 
 * Allocates and frees memory regions using an underlying RegionBuffer.
 * 
 * @code{.cpp}
 * RaeptorCogs::RegionAllocator allocator;
 * size_t offset = allocator.allocate(256);
 * allocator.free(offset, offset + 256);
 * @endcode
 * 
 * @note RegionAllocator main goal is to save memory fragmentation by reusing freed regions.
 */
class RegionAllocator : public RegionBuffer {
public:
    
    // ============================================================================
    //                               PUBLIC METHODS
    // ============================================================================

    /**
     * @brief Allocate a region of the specified size.
     * 
     * @param size The size of the region to allocate.
     * @return size_t The beginning offset of the allocated region, or SIZE_MAX if allocation failed.
     * 
     * @note Allocates the first fitting region found.
     */
    size_t allocate(size_t size) {
        for (auto r : *this) {
            size_t regionSize = r.second - r.first;

            if (regionSize >= size) {
                // Consume part of the region
                size_t offset = r.first;
                this->erase(r.first, r.first + size);
                return offset;
            }
        }
        return SIZE_MAX; // no free region available
    }

    /**
     * @brief Free a previously allocated region.
     * 
     * @param begin The beginning of the region to free.
     * @param end The end of the region to free.
     * 
     * @note Merges with existing regions automatically.
     */
    void free(size_t begin, size_t end) {
        this->push(begin, end);
    }
};
}