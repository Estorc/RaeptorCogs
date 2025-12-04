/** ********************************************************************************
 * @section GAPI_Vulkan_Memory_Overview Overview
 * @file Memory.hpp
 * @brief Vulkan memory management.
 * @details
 * Typical use cases:
 * - Allocating and managing Vulkan device memory
 * *********************************************************************************
 * @section GAPI_Vulkan_Memory_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Core/Memory.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_Memory_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
#include <vulkan/vulkan.h>
#include <vector>

/**
 * @brief Allocate Vulkan device memory for the given data.
 * 
 * @param renderer Pointer to the Vulkan renderer backend.
 * @param data Data for which memory is to be allocated.
 * 
 * @return VkDeviceMemory Handle to the allocated device memory.
 */
template<typename T>
VkDeviceMemory AllocateMemory(RaeptorCogs::GAPI::Vulkan::RendererBackend* renderer, T data);