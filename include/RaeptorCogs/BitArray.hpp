/** ********************************************************************************
 * @section BitArray_Overview Overview
 * @file BitArray.hpp
 * @brief High-level bit array handling utilities.
 * @details
 * Typical use cases:
 * - Handling bit array manipulation and storage
 * *********************************************************************************
 * @section BitArray_Header Header
 * <RaeptorCogs/BitArray.hpp>
 ***********************************************************************************
 * @section BitArray_Metadata Metadata
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
#include <vector>
#include <iostream>
#include <bitset>
#include <cstdint>

namespace RaeptorCogs {

/**
 * @brief BitArray class.
 * 
 * Represents a dynamic array of bits and provides utilities for manipulating them.
 * 
 * @code{.cpp}
 * RaeptorCogs::BitArray bitArray;
 * bitArray.set(5);
 * bool isSet = bitArray.test(5); // true
 * @endcode
 */
class BitArray {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Underlying storage for the bits.
         * 
         * Each uint64_t represents 64 bits.
         * 
         * @note Uses a vector to allow dynamic resizing.
         */
        std::vector<uint64_t> bits;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for BitArray.
         */
        BitArray() = default;

        /**
         * @brief Destructor for BitArray.
         */
        ~BitArray() = default;

        /**
         * @brief Set the bit at the specified index.
         * 
         * @param index The index of the bit to set.
         * @return Reference to the BitArray for chaining.
         * 
         * @note Automatically resizes the underlying storage if the index is out of bounds.
         */
        BitArray& set(size_t index) {
            size_t vecIndex = index / 64;
            size_t bitIndex = index % 64;
            if (vecIndex >= bits.size()) {
                bits.resize(vecIndex + 1, 0);
            }
            bits[vecIndex] |= (uint64_t(1) << bitIndex);
            return *this;
        }

        /**
         * @brief Test if the bit at the specified index is set.
         * 
         * @param index The index of the bit to test.
         * @return true if the bit is set, false otherwise.
         * 
         * @note Returns false if the index is out of bounds.
         */
        bool test(size_t index) {
            size_t vecIndex = index / 64;
            size_t bitIndex = index % 64;
            if (vecIndex >= bits.size()) {
                return false;
            }
            return (bits[vecIndex] & (uint64_t(1) << bitIndex)) != 0;
        }

        /**
         * @brief Clear all bits in the BitArray.
         * 
         * @note Resets the BitArray to an empty state.
         */
        void clear() {
            bits.clear();
        }

        /**
         * @brief Get the underlying data of the BitArray.
         * 
         * @return Vector of uint64_t representing the bits.
         */
        std::vector<uint64_t> data() const {
            return bits;
        }
};

}

namespace std {

/**
 * @brief Stream output operator for BitArray.
 * 
 * @param os Output stream.
 * @param bitArray BitArray instance to output.
 * @return Reference to the output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::BitArray& bitArray) {
    os << "BitArray{";
    for (size_t i = 0; i < bitArray.data().size(); ++i) {
        os << " " << std::bitset<64>(bitArray.data()[i]);
    }
    os << " }";
    return os;
}

}