/** ********************************************************************************
 * @section IO_Font_Overview Overview
 * @file Font.hpp
 * @brief High-level font loading and glyph management.
 * @details
 * Typical use cases:
 * - Loading a font from a TTF file or memory buffer to a SDF atlas
 * - Retrieving glyph data for rendering text
 * - Managing font options such as size and type
 * *********************************************************************************
 * @section IO_Font_Header Header
 * <RaeptorCogs/IO/Font.hpp>
 ***********************************************************************************
 * @section IO_Font_Metadata Metadata
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
#include <RaeptorCogs/IO/String.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <array>
#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace RaeptorCogs {

/**
 * @brief Normal font size constant.
 * 
 * Represents the default font size used for scaling glyphs.
 */
constexpr int NORMAL_FONT_SIZE = 72;

/**
 * @brief Font padding constant.
 * 
 * Represents the padding added around each glyph in the font atlas to prevent texture bleeding.
 */
constexpr int FONT_PADDING = 1;

/**
 * @brief Glyph data structure.
 * 
 * Represents the metrics and texture coordinates of a single glyph in a font atlas.
 * 
 * @code{.cpp}
 *  std::unique_ptr<GlyphData> glyph = std::make_unique<GlyphData>(
 *      packed_glyphs[i].x0 / (float)atlas_w,
 *      packed_glyphs[i].y0 / (float)atlas_h,
 *      packed_glyphs[i].x1 / (float)atlas_w,
 *      packed_glyphs[i].y1 / (float)atlas_h,
 *      (packed_glyphs[i].xoff)/2.0f,
 *      (packed_glyphs[i].yoff)/2.0f,
 *      (packed_glyphs[i].x1 - packed_glyphs[i].x0)/2.0f,
 *      (packed_glyphs[i].y1 - packed_glyphs[i].y0)/2.0f,
 *      packed_glyphs[i].xadvance
 *  );
 * glm::vec4 uvRect = glyph.getUVRect();
 * @endcode
 */
class GlyphData {
    private:
        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief UV texture coordinates of the glyph.
         * 
         * (u0, v0) is the top-left coordinate, (u1, v1) is the bottom-right coordinate.
         * All coordinates are normalized (0.0 to 1.0).
         * 
         * @code{.cpp}
         * glm::vec4 uv = glyph.getUVRect();
         * float u0 = uv.x;
         * float v0 = uv.y;
         * float u1 = uv.z;
         * float v1 = uv.w;
         * @endcode
         */
        glm::vec4 uv;

        /**
         * @brief Offset of the glyph from the cursor position.
         * 
         * Horizontal and vertical offset from cursor position.
         */
        glm::vec2 offset;

        /**
         * @brief Size of the glyph in pixels.
         * 
         * Width and height of the glyph in pixels.
         */
        glm::vec2 size;
        
        /**
         * @brief Horizontal advance after rendering the glyph.
         * 
         * Represents how much to move the cursor for the next glyph.
         */
        float x_advance;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Set the UV rectangle of the glyph.
         * 
         * @param u0 Left U texture coordinate.
         * @param v0 Top V texture coordinate.
         * @param u1 Right U texture coordinate.
         * @param v1 Bottom V texture coordinate.
         * 
         * @code{.cpp}
         * glyph.setUVRect(0.0f, 0.0f, 0.1f, 0.1f);
         * @endcode
         * @note All parameters are normalized texture coordinates (0.0 to 1.0).
         */
        void setUVRect(float u0, float v0, float u1, float v1);

        friend class FontBase;
    public:
        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for GlyphData.
         * 
         * Initializes all metrics to zero.
         */
        GlyphData() : uv(0, 0, 0, 0),
                      offset(0, 0), size(0, 0), x_advance(0) {}
        
        /**
         * @brief Parameterized constructor for GlyphData.
         * 
         * @param u0 Left U texture coordinate.
         * @param v0 Top V texture coordinate.
         * @param u1 Right U texture coordinate.
         * @param v1 Bottom V texture coordinate.
         * @param x_offset Horizontal offset from cursor.
         * @param y_offset Vertical offset from cursor.
         * @param width Width of the glyph in pixels.
         * @param height Height of the glyph in pixels.
         * @param x_advance Horizontal advance after rendering the glyph.
         * @note All size and offset parameters are in pixels.
         */
        GlyphData(float u0, float v0, float u1, float v1,
                  float x_offset, float y_offset, float width, float height, float x_advance)
            : uv(u0, v0, u1, v1), offset(x_offset, y_offset), size(width, height), x_advance(x_advance) {}

        /**
         * @brief Get the UV rectangle of the glyph.
         * 
         * @return UV rectangle (u0, v0, u1, v1) of the glyph.
         */
        glm::vec4 getUVRect() const;

        /**
         * @brief Get the offset of the glyph from the cursor position.
         * 
         * @return Offset (x_offset, y_offset) of the glyph in pixels.
         */
        glm::vec2 getOffset() const;

        /**
         * @brief Get the size of the glyph in pixels.
         * 
         * @return Size (width, height) of the glyph in pixels.
         */
        glm::vec2 getSize() const;

        /**
         * @brief Get the horizontal advance after rendering the glyph.
         * 
         * @return Horizontal advance (x_advance) of the glyph in pixels.
         */
        float getXAdvance() const;
};


/**
 * @brief Enumeration of font types.
 * 
 * Defines the supported font rendering types.
 */
enum class FontType {
    /** Bitmap font type. */
    BITMAP,
    /** Signed Distance Field font type. */
    SDF
};

/**
 * @brief Font options structure.
 * 
 * Used to specify options when loading a font.
 * @code{.cpp}
 * FontOptions options;
 * options.type = FontType::SDF;
 * options.size = 48;
 * Font font("path/to/font.ttf", options);
 * @endcode
 */
struct FontOptions {
    /**
     * @brief Type of the font.
     * 
     * @note Defaults to SDF.
     * @see FontType
     */
    FontType type = FontType::SDF;

    /**
     * @brief Size of the font in pixels.
     * 
     * @note Defaults to NORMAL_FONT_SIZE.
     */
    float size = NORMAL_FONT_SIZE;

    /**
     * @brief Loading priority of the font.
     * 
     * Priority determines the order in which fonts are loaded when multiple fonts are requested simultaneously.
     */
    int priority = 0;
};

/**
 * @brief Base class for font handling.
 * 
 * Provides high-level functionality for loading fonts, managing glyphs, and binding font textures.
 * 
 * @code{.cpp}
 * FontOptions options;
 * options.type = FontType::SDF;
 * options.size = 48;
 * Font font("path/to/font.ttf", options);
 * font.bind();
 * uint32_t textureID = font.getID();
 * GlyphData* glyph = font.getGlyph("A");
 * @endcode
 * @note Should not be used directly. Use the Font wrapper class instead.
 * @see Font
 */
class FontBase {
    private:
        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Callback function invoked when the font is loaded.
         * 
         * Can be set by the user to perform actions once the font is ready.
         */
        std::function<void()> onLoad_;

        /**
         * @brief OpenGL texture ID for the font atlas.
         * 
         * Zero if the font is not loaded.
         */
        uint32_t textureID = 0;

        /**
         * @brief Glyph map storing glyph data indexed by UTF-8 codepoints.
         * 
         * Supports Unicode characters by using a hash map.
         */
        std::unordered_map<uint32_t, std::unique_ptr<GlyphData>> glyphs; // UTF-8 support

        /**
         * @brief Font size in pixels.
         * 
         * Used for scaling glyphs appropriately.
         * @note Corresponds to FontOptions::size.
         */
        float font_size;

        /**
         * @brief Type of the font.
         * 
         * Indicates whether the font is SDF or Bitmap.
         * @note Corresponds to FontOptions::type.
         */
        FontType font_type;


        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================



        /**
         * @brief Private constructor for FontBase.
         * 
         * Use the handler class Font to create and manage FontBase instances.
         */
        FontBase() {};

        /**
         * @brief Factory method to create a FontBase instance from a TTF buffer.
         * 
         * @param ttf_buffer Buffer containing TTF font data.
         * @param options Font loading options.
         * @return Shared pointer to the created FontBase instance.
         */
        static std::shared_ptr<FontBase> create(const FileData& ttf_buffer, FontOptions options = FontOptions());

        /**
         * @brief Factory method to create a FontBase instance from a TTF file path.
         * 
         * @param fontPath Path to the TTF font file.
         * @param options Font loading options.
         * @return Shared pointer to the created FontBase instance.
         */
        static std::shared_ptr<FontBase> create(const std::filesystem::path& fontPath, FontOptions options = FontOptions());

        /**
         * @brief Generate a Signed Distance Field (SDF) font atlas.
         *
         * @param ttf Pointer to the TTF font data in memory.
         * @param data Pointer to the memory buffer where the atlas will be generated.
         * @param atlas_w Width of the atlas in pixels.
         * @param atlas_h Height of the atlas in pixels.
         * @param font_px Font size in pixels.
         * @param num_chars Number of characters to include in the atlas.
         * 
         * @note This function populates the provided data buffer with the generated SDF atlas. 
         */
        void GenerateSDFAtlas(const unsigned char* ttf, unsigned char* data, size_t atlas_w, size_t atlas_h, float font_px, size_t num_chars);

        /**
         * @brief Generate a Bitmap font atlas.
         * 
         * @param ttf Pointer to the TTF font data in memory.
         * @param data Pointer to the memory buffer where the atlas will be generated.
         * @param atlas_w Width of the atlas in pixels.
         * @param atlas_h Height of the atlas in pixels.
         * @param font_px Font size in pixels.
         * @param num_chars Number of characters to include in the atlas.
         * @note This function populates the provided data buffer with the generated Bitmap atlas.
         */
        void GenerateBMPAtlas(const unsigned char* ttf, unsigned char* data, size_t atlas_w, size_t atlas_h, float font_px, size_t num_chars);

        /**
         * @brief Upload the font atlas texture to OpenGL.
         * 
         * @param data Pointer to the font atlas pixel data.
         * @param width Width of the atlas in pixels.
         * @param height Height of the atlas in pixels.
         * @note This function creates an OpenGL texture and uploads the provided pixel data.
         */
        void uploadTexture(const unsigned char* data, size_t width, size_t height);

        friend class Font;
    public:
        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================


        /**
         * @brief Destructor for FontBase.
         * 
         * Cleans up OpenGL resources associated with the font.
         * @note Automatically called when the FontBase instance is destroyed.
         */
        ~FontBase();

        /**
         * @brief Deleted copy constructor and assignment operator.
         * 
         * FontBase instances cannot be copied to prevent resource management issues.
         */
        FontBase(const FontBase&) = delete;
        FontBase& operator=(const FontBase&) = delete;

        /**
         * @brief Defaulted move constructor and assignment operator.
         * 
         * FontBase instances can be moved to transfer ownership of resources.
         */
        FontBase(FontBase&&) noexcept = default;
        FontBase& operator=(FontBase&&) noexcept = default;

        /**
         * @brief Bind the font for rendering.
         * Binds the font's texture to the current OpenGL context.
         */
        void bind() const;

        /**
         * @brief Get the OpenGL texture ID of the font atlas.
         * 
         * @return The OpenGL texture ID.
         * @note Returns 0 if the font is not loaded.
         */
        uint32_t getID() const;

        /**
         * @brief Retrieve glyph data for a specific character.
         * 
         * @param character Pointer to the UTF-8 encoded character.
         * @return GlyphData* Pointer to the GlyphData for the character, or nullptr if not found.
         * @note Supports Unicode characters via UTF-8 encoding.
         */
        GlyphData* getGlyph(const U8Char character) const;

        /**
         * @brief Get the UV rectangle of a specific character's glyph.
         * 
         * @param character Pointer to the UTF-8 encoded character.
         * @return UV rectangle (u0, v0, u1, v1) of the glyph.
         */
        glm::vec4 getGlyphUVRect(const U8Char character) const;

        /**
         * @brief Get the offset of a specific character's glyph.
         * 
         * @param character Pointer to the UTF-8 encoded character.
         * @return Offset (x_offset, y_offset) of the glyph in pixels.
         */
        glm::vec2 getGlyphOffset(const U8Char character) const;

        /**
         * @brief Get the size of a specific character's glyph.
         * 
         * @param character Pointer to the UTF-8 encoded character.
         * @return Size (width, height) of the glyph in pixels
         */
        glm::vec2 getGlyphSize(const U8Char character) const;

        /**
         * @brief Get the horizontal advance of a specific character's glyph.
         * 
         * @param character Pointer to the UTF-8 encoded character.
         * @return Horizontal advance (x_advance) of the glyph in pixels.
         */
        float getGlyphXAdvance(const U8Char character) const;

        /**
         * @brief Get the font size in pixels.
         * 
         * @return Font size in pixels.
         */
        float getFontSize() const;

        /**
         * @brief Check if the font is loaded.
         * 
         * @return true if the font is loaded, false otherwise.
         */
        bool isLoaded() const;
};

/**
 * @brief High-level Font handler class.
 * 
 * Provides a user-friendly interface for loading and managing fonts.
 * Encapsulates a shared pointer to FontBase for resource management.
 * 
 * @code{.cpp}
 * FontOptions options;
 * options.type = FontType::SDF;
 * options.size = 48;
 * Font font("path/to/font.ttf", options);
 * font.bind();
 * uint32_t textureID = font.getID();
 * GlyphData* glyph = font.getGlyph("A");
 * @endcode
 */
class Font {
    private:
        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Shared pointer to the underlying FontBase instance.
         * 
         * Manages the lifetime of the FontBase object.
         * @note Used internally by the Font class.
         */
        std::shared_ptr<FontBase> ptr;

        /**
         * @brief Proxy structure for onLoad callback.
         * 
         * Allows setting a callback function that is invoked when the font is loaded.
         */
        struct OnLoadProxy {
            /**
             * @brief Shared pointer to the FontBase instance.
             * 
             * Used to access the onLoad_ member of FontBase.
             * 
             * @note Used internally by the OnLoadProxy structure.
             */
            std::shared_ptr<FontBase> ptr;

            /**
             * @brief Assignment operator to set the onLoad callback.
             * 
             * @param fn Callback function to be invoked when the font is loaded.
             * 
             * @note If the font is already loaded, the callback is invoked immediately.
             */
            inline void operator=(std::function<void()> fn) {
                if (ptr) ptr->onLoad_ = std::move(fn);
                if (ptr && ptr->isLoaded() && ptr->onLoad_) {
                    ptr->onLoad_();
                }
            }
        };
    public:
        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Font.
         * 
         * Initializes an empty Font instance.
         */
        Font() : ptr(nullptr) {}

        /**
         * @brief Constructor for Font from nullptr.
         * 
         * Initializes an empty Font instance.
         */
        Font(std::nullptr_t) : ptr(nullptr) {}

        /**
         * @brief Constructor for Font from a shared pointer to FontBase.
         * 
         * @param p Shared pointer to an existing FontBase instance.
         * @note Used internally for advanced use cases.
         */
        Font(std::shared_ptr<FontBase> p) : ptr(p) {}

        /**
         * @brief Constructor for Font from a TTF buffer.
         * 
         * @param ttf_buffer Buffer containing TTF font data.
         * @param options Font loading options.
         * 
         * @note Loads the font asynchronously.
         */
        Font(const FileData& ttf_buffer, FontOptions options = FontOptions()) : ptr(FontBase::create(ttf_buffer, options)) {}

        /**
         * @brief Constructor for Font from a TTF file path.
         * 
         * @param fontPath Path to the TTF font file.
         * @param options Font loading options.
         * 
         * @note Loads the font asynchronously.
         */
        Font(std::filesystem::path fontPath, FontOptions options = FontOptions()) : ptr(FontBase::create(fontPath, options)) {}

        /**
         * @brief On-load callback proxy.
         * 
         * Allows setting a callback function that is invoked when the font is loaded.
         * 
         * @code{.cpp}
         * Font font("path/to/font.ttf");
         * font.onLoad = []() {
         *    std::cout << "Font loaded!" << std::endl;
         * };
         * @endcode
         */
        OnLoadProxy onLoad{ ptr };

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline const FontBase& get() const { return *ptr; }

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline const FontBase& operator*() const { return *ptr; }

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline const FontBase* operator->() const { return ptr.get(); }

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline FontBase& get() { return *ptr; }

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline FontBase& operator*() { return *ptr; }

        /**
         * @brief Dereference operators for Font.
         * 
         * Provides access to the underlying FontBase instance.
         * @return Reference or pointer to the FontBase instance.
         */
        inline FontBase* operator->() { return ptr.get(); }

        /**
         * @brief Boolean conversion operator for Font.
         * 
         * Allows checking if the Font instance is valid (i.e., has an associated FontBase).
         * @return true if the Font instance is valid, false otherwise.
         */
        inline explicit operator bool() const noexcept {
            return ptr != nullptr;
        }
};

}

namespace std {

/**
 * @brief Stream output operator for FontOptions.
 * 
 * Outputs the FontOptions in a human-readable format.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::FontOptions& options) {
    return os << "FontOptions{type=" << (options.type == RaeptorCogs::FontType::SDF ? "SDF" : "BITMAP")
              << ", size=" << options.size << "}";
}

/**
 * @brief Stream output operator for FontType.
 * 
 * Outputs the FontType in a human-readable format.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::FontType& type) {
    return os << (type == RaeptorCogs::FontType::SDF ? "SDF" : "BITMAP");
}

/**
 * @brief Stream output operator for GlyphData.
 * 
 * Outputs the GlyphData in a human-readable format.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::GlyphData& glyph) {
    glm::vec4 uv = glyph.getUVRect();
    glm::vec2 offset = glyph.getOffset();
    glm::vec2 size = glyph.getSize();
    float x_advance = glyph.getXAdvance();
    return os << "GlyphData{uv=(" << uv.x << ", " << uv.y << ", " << uv.z << ", " << uv.w << "), "
              << "offset=(" << offset.x << ", " << offset.y << "), "
              << "size=(" << size.x << ", " << size.y << "), "
              << "x_advance=" << x_advance << "}";
}

/**
 * @brief Stream output operator for FontBase.
 * 
 * Outputs the FontBase in a human-readable format.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::FontBase& font) {
    return os << "FontBase{ID=" << font.getID()
              << ", font_size=" << font.getFontSize()
              << ", isLoaded=" << (font.isLoaded() ? "true" : "false") << "}";
}

/**
 * @brief Stream output operator for Font.
 * 
 * Outputs the Font in a human-readable format.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::Font& font) {
    if (font) {
        return os << *font;
    } else {
        return os << "Font{null}";
    }

}

}