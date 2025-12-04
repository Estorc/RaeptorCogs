/** ********************************************************************************
 * @section IO_Images_Overview Overview
 * @file Images.hpp
 * @brief High-level image loading and management.
 * @details
 * Typical use cases:
 * - Loading an image from a file or memory buffer
 * - Creating an empty image
 * - Saving a texture to a PNG file
 * *********************************************************************************
 * @section IO_Images_Header Header
 * <RaeptorCogs/IO/Images.hpp>
 ***********************************************************************************
 * @section IO_Images_Metadata Metadata
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
#include <RaeptorCogs/IO/FileIO.hpp>
#include <iostream>
#include <filesystem>
namespace RaeptorCogs {

/**
 * @brief Image data structure.
 * 
 * Represents an image loaded into memory, including its pixel data, dimensions, and channel count.
 * 
 * @code{.cpp}
 * RaeptorCogs::Image img = RaeptorCogs::LoadImageFromFile("example.png");
 * if (img.data) {
 *    std::cout << "Image loaded: " << img.width << "x" << img.height << " with " << img.channels << " channels." << std::endl;
 * }
 * @endcode
 * 
 * @note The pixel data is managed using a unique_ptr with a custom deleter to ensure proper memory management.
 */
struct Image {

    /**
     * @brief Pixel data of the image.
     * 
     * Pointer to the raw pixel data stored as an array of unsigned char.
     * The data is managed using a unique_ptr with a custom deleter to ensure proper memory management.
     */
    std::unique_ptr<unsigned char[], void(*)(void*)> data;

    /**
     * @brief Width of the image in pixels.
     */
    size_t width;

    /**
     * @brief Height of the image in pixels.
     */
    size_t height;

    /**
     * @brief Number of color channels in the image.
     */
    size_t channels;

    /**
     * @brief Default constructor for an empty image.
     * 
     * Initializes width, height, and channels to zero, and data to nullptr.
     */
    Image();

    /**
     * @brief Parameterized constructor for an image.
     * 
     * @param data Pointer to the pixel data.
     * @param width Width of the image in pixels.
     * @param height Height of the image in pixels.
     * @param channels Number of color channels in the image.
     */
    Image(unsigned char* data, size_t width, size_t height, size_t channels);

    /**
     * @brief Destructor for the Image structure.
     * Cleans up the pixel data using the custom deleter.
     */
    ~Image() = default;

    /**
     * @brief Deleted copy constructor and assignment operator to prevent copying.
     * Images manage dynamic memory and should not be copied.
     */
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    /**
     * @brief Defaulted move constructor and assignment operator for efficient transfer of resources.
     * Allows moving Image instances without copying the pixel data.
     */
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    /**
     * @brief Check if the image is fully opaque.
     * @return true if all pixels are fully opaque, false otherwise.
     * 
     * @note An image is considered opaque if all alpha channel values are 255.
     */
    bool isOpaque() const {
        if (!data || channels < 4) return true; // Not enough channels for opacity
        for (size_t i = 0; i < width * height * channels; i += channels) {
            if (data[i + 3] < 255) {
                return false; // If any pixel is not fully opaque
            }
        }
        return true; // All pixels are fully opaque
    }
};

/**
 * @brief Load a texture from an image file.
 * 
 * @param filename Path to the image file.
 * @return OpenGL texture ID.
 * @note Returns 0 if loading fails.
 */
uint32_t LoadTexture(const std::filesystem::path& filename);

/**
 * @brief Load an image from a URL.
 * 
 * @param url URL of the image.
 * @return Loaded Image object.
 * @note Returns an empty Image if loading fails (and if __EMSCRIPTEN__ is defined).
 */
Image LoadImageFromURL(const std::filesystem::path& url);

/**
 * @brief Load an image from memory.
 * 
 * @param data Byte data of the image file.
 * @param s_width Optional desired width to resize the image to. Set to 0 to keep original width.
 * @param s_height Optional desired height to resize the image to. Set to 0 to keep original height.
 * @return Loaded Image object.
 * @note Returns an empty Image if loading fails.
 */
Image LoadImageFromMemory(const FileData& data, size_t s_width = 0, size_t s_height = 0);

/**
 * @brief Load an image from a file.
 * 
 * @param filename Path to the image file.
 * @param s_width Optional desired width to resize the image to. Set to 0 to keep original width.
 * @param s_height Optional desired height to resize the image to. Set to 0 to keep original height.
 * @return Loaded Image object.
 * @note Returns an empty Image if loading fails.
 */
Image LoadImageFromFile(const std::filesystem::path& filename, size_t s_width = 0, size_t s_height = 0);

/**
 * @brief Create an empty image with specified dimensions.
 * 
 * @param width Width of the image in pixels.
 * @param height Height of the image in pixels.
 * @return Created Image object with allocated pixel data.
 * @note The created image has 4 channels (RGBA) initialized to zero.
 */
Image CreateImage(size_t width, size_t height);

/**
 * @brief Save an OpenGL texture to a PNG file.
 * 
 * @param textureID OpenGL texture ID.
 * @param width Width of the texture in pixels.
 * @param height Height of the texture in pixels.
 * @param filename Path to save the PNG file.
 * @note Overwrites the file if it already exists.
 */
void saveTextureToPNG(uint32_t textureID, size_t width, size_t height, const std::filesystem::path& filename);
}