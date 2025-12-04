/** ********************************************************************************
 * @section Sprite_Overview Overview
 * @file Sprite.hpp
 * @brief High-level sprite handling utilities.
 * @details
 * Typical use cases:
 * - Managing sprite instances and rendering.
 * *********************************************************************************
 * @section Sprite_Header Header
 * <RaeptorCogs/Sprite.hpp>
 ***********************************************************************************
 * @section Sprite_Metadata Metadata
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
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Flags.hpp>

namespace RaeptorCogs::Singletons {
class Renderer;
}
namespace RaeptorCogs {

/**
 * @brief Class representing a 2D sprite.
 * 
 * Manages texture and rendering for a 2D sprite. Inherits from
 * TransformableGraphic2D to provide transformation capabilities.
 * 
 * @see TransformableGraphic2D
 */
class Sprite2D : public TransformableGraphic2D {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Texture associated with the sprite.
         * 
         * Holds the texture used for rendering the sprite.
         * 
         * @note Initialized to nullptr.
         */
        Texture texture = nullptr;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Sprite2D with a texture.
         * 
         * @param texture Texture to associate with the sprite.
         */
        Sprite2D(Texture &texture);
        
        /**
         * @brief Default constructor for Sprite2D.
         */
        Sprite2D() {}

        /**
         * @brief Destructor for Sprite2D.
         */
        ~Sprite2D() = default;

        /**
         * @brief Compute instance data for the sprite.
         * 
         * @param instanceAllocator Reference to the instance allocator.
         * @param mode Mode for computing instance data.
         * @return true if updates were made, false otherwise.
         */
        bool computeInstanceData(GAPI::Common::InstanceAllocator &instanceAllocator, ComputeInstanceDataMode mode = ComputeInstanceDataMode::NONE) override;

        /**
         * @brief Bind the sprite for rendering.
         * 
         * @note Overrides the bind method from Graphic2D.
         */
        void bind() const override;

        /**
         * @brief Get the unique ID of the sprite.
         * 
         * @return Unique ID of the sprite.
         */
        GLuint getID() const override;

        /**
         * @brief Check if the sprite is opaque.
         * 
         * @return true if the sprite is opaque, false otherwise.
         */
        bool isOpaque() const override;

        /**
         * @brief Check if the sprite is visible.
         * 
         * @return true if the sprite is visible, false otherwise.
         */
        bool isVisible() const override;

        /**
         * @brief Get the texture associated with the sprite.
         * 
         * @return Texture associated with the sprite.
         */
        Texture getTexture() const override;

        /**
         * @brief Set the texture associated with the sprite.
         * 
         * @param texture Texture to associate with the sprite.
         */
        void setTexture(Texture &texture);
};

}