/** ********************************************************************************
 * @section Random_Overview Overview
 * @file Random.hpp
 * @brief High-level random number generation utilities.
 * @details
 * Typical use cases:
 * - Generating random integers within a specified range.
 * - Generating random floating-point numbers within a specified range.
 * *********************************************************************************
 * @section Random_Header Header
 * <RaeptorCogs/Random.hpp>
 ***********************************************************************************
 * @section Random_Metadata Metadata
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
#include <RaeptorCogs/Singleton.hpp>

namespace RaeptorCogs {
namespace Singletons {

/**
 * @brief Random singleton class.
 * 
 * Provides random number generation functionalities.
 * 
 * @code{.cpp}
 * int randInt = RaeptorCogs::Random().getInt(1, 10);
 * float randFloat = RaeptorCogs::Random().getFloat(0.0f, 1.0f);
 * @endcode
 */
class Random {
    private:

        // ============================================================================
        //                             PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Private constructor for the Random singleton.
         */
        Random() = default;

        friend SingletonAccessor<Random>;

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Deleted copy constructor
         */
        Random(const Random&) = delete;
        
        /**
         * @brief Deleted assignment operator
         */
        Random& operator=(const Random&) = delete;

        /**
         * @brief Generate a random integer in the range [min, max].
         * 
         * @param min The minimum value (inclusive).
         * @param max The maximum value (inclusive).
         * @return A random integer between min and max.
         * 
         * @code{.cpp}
         * int randInt = RaeptorCogs::Random().getInt(1, 10);
         * @endcode
         */
        int getInt(int min, int max);

        /**
         * @brief Generate a random float in the range [min, max).
         * 
         * @param min The minimum value (inclusive).
         * @param max The maximum value (exclusive).
         * @return A random float between min and max.
         * 
         * @code{.cpp}
         * float randFloat = RaeptorCogs::Random().getFloat(0.0f, 1.0f);
         * @endcode
         */
        float getFloat(float min, float max);

};

}
}