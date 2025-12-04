/** ********************************************************************************
 * @section BitOp_Overview Overview
 * @file BitOp.hpp
 * @brief High-level bit operation utilities.
 * @details
 * Typical use cases:
 * - Calculating the next power of 2 for a given number
 * *********************************************************************************
 * @section BitOp_Header Header
 * <RaeptorCogs/BitOp.hpp>
 ***********************************************************************************
 * @section BitOp_Metadata Metadata
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
#include <stdint.h>
namespace RaeptorCogs {

/**
 * @brief Calculate the next power of 2 greater than or equal to n.
 * 
 * @param n Input number.
 * @return The next power of 2 greater than or equal to n.
 * 
 * @code{.cpp}
 * uint64_t result = RaeptorCogs::NextPowerOf2(300); // result = 512
 * @endcode
 */
uint64_t NextPowerOf2(uint64_t n);

}