/** ********************************************************************************
 * @section GAPI_Vulkan_Vulkan_Overview Overview
 * @file Vulkan.hpp
 * @brief Vulkan core functionality.
 * @details
 * Typical use cases:
 * - Providing core Vulkan utility functions
 * *********************************************************************************
 * @section GAPI_Vulkan_Vulkan_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Core/Vulkan.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_Vulkan_Metadata Metadata
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
#include <vulkan/vulkan.h>
#include <vector>

/**
 * @brief Find a supported depth format.
 * 
 * @param physicalDevice The Vulkan physical device.
 * @return VkFormat The first supported format from the candidates.
 * @throws std::runtime_error if no supported format is found.
 * 
 * @code{.cpp}
 * VkFormat depthFormat = RaeptorCogs::GAPI::Vulkan::findDepthFormat(physicalDevice);
 * @endcode
 */
VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

/**
 * @brief Find the best surface format for the given physical device and surface.
 * 
 * @param physicalDevice The Vulkan physical device.
 * @param surface The Vulkan surface.
 * @param candidates Optional list of candidate formats to consider.
 * @return VkSurfaceFormatKHR The best matching surface format.
 * 
 * @code{.cpp}
 * VkSurfaceFormatKHR surfaceFormat = RaeptorCogs::GAPI::Vulkan::findBestSurfaceFormat(physicalDevice, surface);
 * @endcode
 */
VkSurfaceFormatKHR findBestSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<VkFormat>& candidates = {
    VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8_UNORM,
    VK_FORMAT_R8G8B8_UNORM
});

/**
 * @brief Check if the requested validation layers are supported.
 * 
 * @param validationLayers List of validation layer names to check.
 * @return true if all requested layers are supported, false otherwise.
 * 
 * @code{.cpp}
 * std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
 * bool supported = RaeptorCogs::GAPI::Vulkan::checkValidationLayerSupport(layers);
 * @endcode
 */
bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);

/**
 * @brief Rate the suitability of a Vulkan physical device.
 * 
 * @param device The Vulkan physical device to rate.
 * @return int Suitability score of the device.
 * 
 * @code{.cpp}
 * int score = RaeptorCogs::GAPI::Vulkan::rateDeviceSuitability(physicalDevice);
 * @endcode
 */
int rateDeviceSuitability(VkPhysicalDevice device);

/**
 * @brief Check if a given VkFormat is an sRGB format.
 * 
 * @param format The VkFormat to check.
 * @return true if the format is sRGB, false otherwise.
 * 
 * @code{.cpp}
 * bool isSRGB = RaeptorCogs::GAPI::Vulkan::IsSRGBFormat(VK_FORMAT_R8G8B8A8_SRGB);
 * @endcode
 */
inline bool IsSRGBFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_SRGB:
        case VK_FORMAT_R8G8_SRGB:
        case VK_FORMAT_R8G8B8_SRGB:
        case VK_FORMAT_B8G8R8_SRGB:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_SRGB:
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        case VK_FORMAT_BC2_SRGB_BLOCK:
        case VK_FORMAT_BC3_SRGB_BLOCK:
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return true;
        default:
            return false;
    }
}