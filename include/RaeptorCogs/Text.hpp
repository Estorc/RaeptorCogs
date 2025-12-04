/** ********************************************************************************
 * @section Text_Overview Overview
 * @file Text.hpp
 * @brief High-level text handling utilities.
 * @details
 * Typical use cases:
 * - Managing text rendering and glyph handling.
 * *********************************************************************************
 * @section Text_Header Header
 * <RaeptorCogs/Text.hpp>
 ***********************************************************************************
 * @section Text_Metadata Metadata
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
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <string>

namespace RaeptorCogs::Singletons {
class Renderer;
}
namespace RaeptorCogs {

class Text2D;
class Glyph;

/**
 * @brief Class representing a single glyph (character) in a text.
 * 
 * Inherits from TransformableGraphic2D to provide transformation capabilities.
 * 
 * @see TransformableGraphic2D
 */
class Glyph : public TransformableGraphic2D {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the parent Text2D object.
         * 
         * Used to access font and rendering information.
         * 
         * @note This pointer should not be null.
         */
        Text2D *text;

        /**
         * @brief The character represented by this glyph.
         * 
         * Used to identify the glyph in the font.
         * 
         * @note Changing this character will require updating the glyph's rendering data.
         */
        U8Char character; // The character represented by this glyph

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Glyph.
         * 
         * @param text Reference to the parent Text2D object.
         * @param character The character represented by this glyph.
         * @param advance The advance vector for the glyph.
         * 
         * @note Initializes the glyph with the specified character and advance.
         */
        Glyph(Text2D &text, const U8Char &character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));

        /**
         * @brief Default constructor for Glyph.
         */
        Glyph();

        /**
         * @brief Destructor for Glyph.
         */
        ~Glyph() override = default;

        /**
         * @brief Set the character for this glyph.
         * 
         * @param character The new character.
         * @param advance The advance vector for the glyph.
         */
        void setCharacter(const U8Char &character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));

        /**
         * @brief Compute instance data for this glyph.
         * 
         * @param instanceAllocator Reference to the InstanceAllocator.
         * @param mode The compute instance data mode.
         * @return true if instance data was computed, false otherwise.
         * 
         * @note Overrides the base class method.
         */
        bool computeInstanceData(GAPI::Common::InstanceAllocator &instanceAllocator, ComputeInstanceDataMode mode = ComputeInstanceDataMode::NONE) override;
        
        /**
         * @brief Bind this glyph for rendering.
         * 
         * @note Overrides the base class method.
         */
        void bind() const override;

        /**
         * @brief Get the unique ID of this glyph.
         * 
         * @return Unique ID of the glyph.
         */
        GLuint getID() const override;

        /**
         * @brief Check if the glyph is opaque.
         * 
         * @note Overrides the base class method.
         */
        bool isOpaque() const override { return false; }
};

/**
 * @brief Enumeration for text word wrapping modes.
 * 
 * Defines how text should be wrapped when it exceeds a certain width.
 */
enum class TextWordWrap {
    /** No word wrapping */
    NONE,
    /** Wrap at word boundaries */
    WORD,
    /** Wrap at character boundaries */
    CHARACTER
};

/**
 * @brief Enumeration for text alignment options.
 * 
 * Defines how text should be aligned within its bounding box.
 */
enum class TextAlignment {
    /** Align text to the left */
    LEFT,
    /** Center align text */
    CENTER,
    /** Align text to the right */
    RIGHT
};

/**
 * @brief Flags for Text2D class.
 * 
 * Defines various states for text rendering.
 */
enum class TextFlags : uint32_t {
    /** No flags set */
    NONE = 0,
    /** Flag to indicate if the text needs to be rebuilt */
    TEXT_DIRTY = 1 << 0,
};

/**
 * @brief Constant for normal font size.
 * 
 * Used as a default size for text rendering.
 * 
 * @note Size is in points.
 */
class Text2D : public TransformableGraphic2D, public FlagSet<TextFlags> {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Font used for rendering the text.
         * 
         * @note Changing the font will require rebuilding the text.
         */
        Font font = nullptr;

        /**
         * @brief Vector of glyphs representing the text.
         * 
         * @note Each glyph corresponds to a character in the text.
         */
        std::vector<std::shared_ptr<Glyph>> glyphs;

        /**
         * @brief Size of the text.
         * 
         * @note Size is in points.
         */
        float textSize = NORMAL_FONT_SIZE;

        /**
         * @brief Content of the text.
         * 
         * @note Changing the content will require rebuilding the text.
         */
        U8String content;

        /**
         * @brief Width for word wrapping.
         * 
         * @note Used in conjunction with wordWrapType.
         */
        float wordWrapWidth = 0.0f;

        /**
         * @brief Word wrapping mode.
         * 
         * @note Defines how text should be wrapped.
         */
        TextWordWrap wordWrapType = TextWordWrap::NONE;

        /**
         * @brief Text alignment mode.
         * 
         * @note Defines how text should be aligned.
         */
        TextAlignment alignment = TextAlignment::LEFT;
        
        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Set the text dirty flag.
         * 
         * @param dirty true to mark the text as dirty, false otherwise.
         * 
         * @note Used internally to manage text state.
         */
        void setTextDirty(bool dirty);

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Text2D.
         * 
         * @param font Reference to the Font object.
         * @param content The text content.
         * 
         * @note Initializes the text with the specified font and content.
         */
        Text2D(Font &font, const U8String &content);

        /**
         * @brief Constructor for Text2D with empty content.
         * 
         * @param font Reference to the Font object.
         * 
         * @note Initializes the text with the specified font and empty content.
         */
        Text2D(Font &font) : Text2D(font, "") {}

        /**
         * @brief Default constructor for Text2D.
         */
        Text2D() : font(nullptr) {}

        /**
         * @brief Destructor for Text2D.
         */
        ~Text2D() override;

        /**
         * @brief Set the renderer for this text.
         * 
         * @param renderer Pointer to the Renderer singleton.
         * 
         * @note Overrides the base class method for adding the glyphs.
         */
        void setRenderer(Singletons::Renderer *renderer) override;

        /**
         * @brief Rebuild the text glyphs.
         * 
         * @note Called when the text content, font, or size changes.
         */
        virtual void rebuildText();
        
        /**
         * @brief Bind this text for rendering.
         * 
         * @note Overrides the base class method.
         */
        void bind() const override;

        /**
         * @brief Get the unique ID of this text.
         * 
         * @return Unique ID of the text.
         */
        GLuint getID() const override;

        /**
         * @brief Check if the text is opaque.
         * 
         * @return true if the text is opaque, false otherwise.
         */
        bool isOpaque() const override { return false; }

        /**
         * @brief Check if the text is visible.
         * 
         * @return true if the text is visible, false otherwise.
         */
        bool isVisible() const override;

        /**
         * @brief Check if the text needs to be rebuilt.
         * 
         * @return true if the text is dirty, false otherwise.
         */
        bool isTextDirty() const;

        /**
         * @brief Set the Z-index of the text.
         * 
         * @param z The new Z-index value.
         */
        void setZIndex(float z) override;

        /**
         * @brief Set the content of the text.
         * 
         * @param content The new text content.
         * 
         * @note Changing these properties will mark the text as dirty.
         */
        void setContent(const U8String &content);

        /**
         * @brief Set the font for the text.
         * 
         * @param font Reference to the new Font object.
         * 
         * @note Changing the font will mark the text as dirty.
         */
        void setFont(Font &font);

        /**
         * @brief Set the word wrapping mode and width.
         * 
         * @param wrap The word wrapping mode.
         * @param width The width for word wrapping.
         * 
         * @note Changing these properties will mark the text as dirty.
         */
        void setWordWrap(TextWordWrap wrap, float width = 0.0f);

        /**
         * @brief Set the text alignment mode.
         * 
         * @param align The text alignment mode.
         * 
         * @note Changing the alignment will mark the text as dirty.
         */
        void setAlignment(TextAlignment align);

        /**
         * @brief Set the size of the text.
         * 
         * @param size The new text size in points.
         * 
         * @note Changing the size will mark the text as dirty.
         */
        void setTextSize(float size);

        /**
         * @brief Get the content of the text.
         * 
         * @return Reference to the text content.
         */
        const U8String& getContent() const;

        /**
         * @brief Get the font used for the text.
         * 
         * @return Reference to the Font object.
         */
        Font getFont() const;

        /**
         * @brief Get the word wrapping mode.
         * 
         * @return The word wrapping mode.
         */
        TextWordWrap getWordWrapType() const;

        /**
         * @brief Get the word wrap width.
         * 
         * @return The word wrap width.
         */
        float getWordWrapWidth() const;

        /**
         * @brief Get the text alignment mode.
         * 
         * @return The text alignment mode.
         */
        TextAlignment getAlignment() const;

        /**
         * @brief Get the size of the text.
         * 
         * @return The text size in points.
         */
        float getTextSize() const;

        /**
         * @brief Measure the size of the rendered text.
         * 
         * @return A glm::vec2 representing the width and height of the text.
         * 
         * @note Takes into account word wrapping and alignment.
         */
        glm::vec2 measureTextSize() const;

        /**
         * @brief Measure the width of a single line of text.
         * 
         * @param text The line of text to measure.
         * @return The width of the line in pixels.
         * 
         * @note Does not consider word wrapping.
         */
        float measureLineWidth(const U8String& text) const;
};

}