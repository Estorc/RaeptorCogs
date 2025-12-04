/** ********************************************************************************
 * @section IO_Texture_Overview Overview
 * @file Texture.hpp
 * @brief High-level texture handling utilities.
 * @details
 * Typical use cases:
 * - Handling texture loading, manipulation, and atlas creation
 * 
 * TODO:
 * - Constant atlas loader
 * - Move uv rects to a separate SSBO for skipping rebuilds
 * - More texture formats
 * *********************************************************************************
 * @section IO_Texture_Header Header
 * <RaeptorCogs/IO/Texture.hpp>
 ***********************************************************************************
 * @section IO_Texture_Metadata Metadata
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
#include <RaeptorCogs/GAPI/GL/Constants.hpp>
#include <RaeptorCogs/GAPI/Common/Ressources/Object.hpp>
#include <RaeptorCogs/GAPI/Common/Ressources/TextureData.hpp>
#include <RaeptorCogs/IO/Images.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/Singleton.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <map>
#include <RaeptorCogs/External/stb/stb.hpp>
#include <filesystem>



namespace RaeptorCogs {

/**
 * @brief Common atlas dimensions.
 */
constexpr unsigned int COMMON_ATLAS_SIZE = 1024;

/**
 * @brief Padding around textures in the atlas to prevent bleeding.
 */
constexpr unsigned int ATLAS_PADDING = 1;


/**
 * @brief Texture atlas flags enumeration.
 * 
 * Defines flags for texture atlas management.
 */
enum class TextureAtlasFlags : uint32_t {
    NONE = 0,
    NEEDS_REBUILD = 1 << 0, // Flag to indicate if the atlas needs to be rebuilt
};

class TextureBase;

/**
 * @brief Type key for texture atlas map.
 * 
 * Used to uniquely identify texture atlases based on their filtering options.
 */
using TextureAtlasTypeKey = std::tuple<unsigned int, unsigned int>; // Key type for texture atlas map

/**
 * @brief Texture atlas class.
 * 
 * Manages a collection of textures packed into a single atlas texture.
 */
class TextureAtlas {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Size of the texture atlas.
         * 
         * (width, height) in pixels.
         */
        glm::ivec2 size; // Size of the atlas

        /**
         * @brief List of textures in the atlas.
         * @see TextureBase
         */
        std::vector<TextureBase *> textures;

        /**
         * @brief GAPI texture handler for the atlas.
         * 
         * @see GAPI::Common::TextureData
         */
        GAPI::ObjectHandler<GAPI::Common::TextureData> glTexture;

        /**
         * @brief STB rect pack context for packing textures.
         */
        stbrp_context ctx;

        /**
         * @brief Nodes used for packing textures.
         */
        std::vector<stbrp_node> nodes;

        /**
         * @brief Texture atlas flags.
         * 
         * @see TextureAtlasFlags
         */
        TextureAtlasFlags flags = TextureAtlasFlags::NONE;

        /**
         * @brief Minification filter for the atlas texture.
         */
        unsigned int minFilter = GL_LINEAR_MIPMAP_NEAREST;

        /**
         * @brief Magnification filter for the atlas texture.
         */
        unsigned int magFilter = GL_LINEAR;

        /**
         * @brief Amount of free space in the atlas.
         * 
         * @note Used to quickly check if a new texture can fit.
         */
        int freeSpace = 0;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for TextureAtlas.
         */
        TextureAtlas();

        /**
         * @brief Constructor for TextureAtlas with specified size and filtering options.
         * 
         * @param size Size of the atlas (width, height) in pixels.
         * @param minFilter Minification filter for the atlas texture.
         * @param magFilter Magnification filter for the atlas texture.
         * 
         * @note Textures added to the atlas will be packed using stb_rect_pack.
         */
        TextureAtlas(glm::ivec2 size, unsigned int minFilter = GL_LINEAR_MIPMAP_NEAREST, unsigned int magFilter = GL_LINEAR);

        /**
         * @brief Bind the atlas texture for rendering.
         * 
         * @see GAPI::Common::TextureData::bind()
         */
        void bind();

        /**
         * @brief Unbind the atlas texture.
         * 
         * @see GAPI::Common::TextureData::unbind()
         */
        void unbind() const;

        /**
         * @brief Upload a texture to the atlas at the specified position.
         * 
         * @param x X position in the atlas to upload the texture.
         * @param y Y position in the atlas to upload the texture.
         * @param width Width of the texture to upload.
         * @param height Height of the texture to upload.
         * @param data Pointer to the pixel data of the texture.
         * @param newAtlas Whether this is a new atlas upload (true) or an update (false).
         * 
         * @note The pixel data should be in RGBA format.
         * @see GAPI::Common::TextureData::build()
         */
        void uploadTexture(GLint x, GLint y, GLint width, GLint height, const void *data, bool newAtlas);

        /**
         * @brief Try to add a texture to the atlas.
         * 
         * @param texture Pointer to the texture to add.
         * @param width Width of the texture in pixels.
         * @param height Height of the texture in pixels.
         * @return true if the texture was successfully added, false otherwise.
         * 
         * @note The texture's rectangle and UV rectangle will be set upon successful addition.
         */
        bool tryAddTexture(TextureBase *texture, int width, int height);

        /**
         * @brief Remove a texture from the atlas.
         * 
         * @param texture Reference to the texture to remove.
         * 
         * @warning The atlas will be repacked after removal, which may be a costly operation.
         * @note This method is called automatically when a texture is destroyed.
         */
        void removeTexture(const TextureBase &texture);

        /**
         * @brief Get the texture ID of the atlas.
         * 
         * @return The GAPI texture ID of the atlas.
         */
        uint32_t getID() const;

        /**
         * @brief Get the width of the atlas.
         * 
         * @return The width of the atlas in pixels.
         */
        int getWidth() const;

        /**
         * @brief Get the height of the atlas.
         * 
         * @return The height of the atlas in pixels.
         */
        int getHeight() const;

        /**
         * @brief Get the amount of free space in the atlas.
         * 
         * @return The amount of free space in pixels.
         */
        int getFreeSpace() const;

        /**
         * @brief Check if the atlas GPU linking needs to be rebuilt.
         * 
         * @return true if the atlas needs to be rebuilt, false otherwise.
         * @note This tell the renderer to rebuild uv rectangles in the GPU.
         */
        bool needsRebuild() const;

        /**
         * @brief Get the type key for the texture atlas.
         * 
         * @return The TextureAtlasTypeKey representing the atlas's filtering options.
         */
        TextureAtlasTypeKey getTypeKey() const { 
            return std::make_tuple(this->minFilter, this->magFilter);
        }
};


/**
 * @brief Texture loading options structure.
 * 
 * Defines options for loading textures.
 * 
 * @note Default values are provided for all options.
 */
struct TextureOptions {
    /**
     * @brief Minification filter for the texture.
     */
    unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
    
    /**
     * @brief Magnification filter for the texture.
     */
    unsigned int magFilter = GL_LINEAR;

    /**
     * @brief Desired width after scaling. 0 means original width.
     */
    unsigned int s_width = 0;

    /**
     * @brief Desired height after scaling. 0 means original height.
     */
    unsigned int s_height = 0;

    /**
     * @brief Loading priority of the texture.
     * 
     * Higher priority textures are loaded before lower priority ones.
     */
    int priority = 0;
};


/**
 * @brief Base texture class.
 * 
 * Provides common functionality for textures, including atlas management.
 * 
 * @code{.cpp}
 * RaeptorCogs::Texture tex("path/to/texture.png");
 * tex.bind();
 * uint32_t textureID = tex.getID();
 * @endcode
 * @note Should not be used directly. Use the Texture wrapper class instead.
 * @see Texture
 */
class TextureBase {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Callback function invoked when the texture is loaded.
         * 
         * Can be set by the user to perform actions once the texture is ready.
         */
        std::function<void()> onLoad_;

        /**
         * @brief Weak pointer to the texture atlas this texture belongs to.
         * 
         * @see TextureAtlas
         */
        std::weak_ptr<TextureAtlas> atlas;

        /**
         * @brief Rectangle of the texture in the atlas.
         * 
         * (x, y, width, height) in pixels.
         */
        glm::vec4 rect;

        /**
         * @brief UV rectangle of the texture in the atlas.
         * 
         * (u, v, u_width, v_height) in normalized coordinates [0, 1].
         */
        glm::vec4 uvRect;

        /**
         * @brief Opaqueness flag of the texture.
         * 
         * true if the texture is fully opaque, false otherwise.
         */
        bool opaque;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Upload image data to the texture.
         * 
         * @param img Image structure containing pixel data and dimensions.
         */
        void upload(const Image &img);

        /**
         * @brief Private constructor for TextureBase.
         * 
         * Use the handler class Texture to create and manage TextureBase instances.
         */
        TextureBase() : onLoad_(nullptr), atlas(), opaque(false) {}

        /**
         * @brief Factory method to create a TextureBase from an Image.
         * 
         * @param img Image structure containing pixel data and dimensions.
         * @return Shared pointer to the created TextureBase instance.
         */
        static std::shared_ptr<TextureBase> create(const Image &img);

        /**
         * @brief Factory method to create a TextureBase with specified dimensions.
         * 
         * @param width Width of the texture in pixels.
         * @param height Height of the texture in pixels.
         * @param options Texture loading options.
         * @return Shared pointer to the created TextureBase instance.
         */
        static std::shared_ptr<TextureBase> create(unsigned int width, unsigned int height, TextureOptions options = TextureOptions());

        /**
         * @brief Factory method to create a TextureBase from file data.
         * 
         * @param fileData Vector containing raw file data.
         * @param options Texture loading options.
         * @return Shared pointer to the created TextureBase instance.
         */
        static std::shared_ptr<TextureBase> create(const FileData &fileData, TextureOptions options = TextureOptions());

        /**
         * @brief Factory method to create a TextureBase from a file path.
         * 
         * @param filepath Path to the texture file.
         * @param options Texture loading options.
         * @return Shared pointer to the created TextureBase instance.
         */
        static std::shared_ptr<TextureBase> create(const std::filesystem::path& filepath, TextureOptions options = TextureOptions());

        /**
         * @brief Set the rectangle of the texture in the atlas.
         * 
         * @param rect Rectangle (x, y, width, height) in pixels.
         */
        void setRect(const glm::vec4 &rect);

        /**
         * @brief Set the UV rectangle of the texture in the atlas.
         * 
         * @param uv UV rectangle (u, v, u_width, v_height) in normalized coordinates [0, 1].
         */
        void setUVRect(const glm::vec4 &uv);

        friend class Texture;
        friend class TextureAtlas;

    public:
        
        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Destructor for TextureBase.
         * 
         * Cleans up resources and removes the texture from its atlas if applicable.
         * @see TextureAtlas::removeTexture()
         */
        ~TextureBase();

        /**
         * @brief Bind the texture for rendering.
         * 
         * @see TextureAtlas::bind()
         */
        void bind() const;

        /**
         * @brief Check if the texture is fully opaque.
         * 
         * @return true if the texture is fully opaque, false otherwise.
         */
        bool isOpaque() const;

        /**
         * @brief Check if the texture has been loaded.
         * 
         * @return true if the texture is loaded, false otherwise.
         */
        bool isLoaded() const;

        /**
         * @brief Set the texture atlas for this texture.
         * 
         * @param atlas Shared pointer to the texture atlas.
         */
        void setAtlas(std::shared_ptr<TextureAtlas> atlas);

        /**
         * @brief Get the X position of the texture in the atlas.
         * 
         * @return X position in pixels.
         */
        float getX() const;

        /**
         * @brief Get the Y position of the texture in the atlas.
         * 
         * @return Y position in pixels.
         */
        float getY() const;

        /**
         * @brief Get the width of the texture in the atlas.
         * 
         * @return Width in pixels.
         */
        float getWidth() const;

        /**
         * @brief Get the height of the texture in the atlas.
         * 
         * @return Height in pixels.
         */
        float getHeight() const;

        /**
         * @brief Get the rectangle of the texture in the atlas.
         * 
         * @return Rectangle (x, y, width, height) in pixels.
         */
        glm::vec4 getRect();

        /**
         * @brief Get the UV rectangle of the texture in the atlas.
         * 
         * @return UV rectangle (u, v, u_width, v_height) in normalized coordinates [0, 1].
         */
        glm::vec4 getUVRect();

        /**
         * @brief Get the texture atlas this texture belongs to.
         * 
         * @return Pointer to the texture atlas, or nullptr if not assigned.
         */
        TextureAtlas *getAtlas() const;

        /**
         * @brief Get the GAPI texture ID of this texture.
         * 
         * @return The GAPI texture ID, or 0 if not loaded.
         */
        uint32_t getID() const;

        /**
         * @brief Check if the texture GPU linking needs to be rebuilt.
         * 
         * @return true if the texture needs to be rebuilt, false otherwise.
         * @note This tell the renderer to rebuild uv rectangles in the GPU.
         */
        bool needsRebuild() const;
};

/**
 * @brief High-level texture handler class.
 * 
 * Wraps around TextureBase to provide easy-to-use texture management.
 * 
 * @code{.cpp}
 * RaeptorCogs::Texture tex("path/to/texture.png");
 * tex.bind();
 * uint32_t textureID = tex.getID();
 * @endcode
 */
class Texture {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Shared pointer to the underlying TextureBase instance.
         * 
         * Manages the lifetime of the texture.
         * @note Used internally by the Texture class.
         */
        std::shared_ptr<TextureBase> ptr;

        /**
         * @brief Proxy structure for setting the onLoad callback.
         * 
         * Allows users to set a callback function that is invoked when the texture is loaded.
         */
        struct OnLoadProxy {
            /**
             * @brief Shared pointer to the TextureBase instance.
             * 
             * Used to access and set the onLoad_ callback.
             * 
             * @note Used internally by the OnLoadProxy structure.
             */
            std::shared_ptr<TextureBase> ptr;

            /**
             * @brief Assignment operator to set the onLoad callback.
             * 
             * @param fn Callback function to be invoked when the texture is loaded.
             * 
             * @note If the texture is already loaded, the callback is invoked immediately.
             */
            void operator=(std::function<void()> fn) {
                ptr->onLoad_ = std::move(fn);
                if (ptr->isLoaded() && ptr->onLoad_) {
                    ptr->onLoad_();
                }
            }
        };
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Texture.
         * 
         * Initializes the texture to a null state.
         */
        Texture() : ptr(nullptr) {}

        /**
         * @brief Constructor for Texture from nullptr.
         * 
         * Initializes the texture to a null state.
         */
        Texture(nullptr_t) : ptr(nullptr) {}

        /**
         * @brief Constructor for Texture from a shared pointer to TextureBase.
         * 
         * @param p Shared pointer to the TextureBase instance.
         */
        Texture(std::shared_ptr<TextureBase> p) : ptr(p) {}

        /**
         * @brief Constructor for Texture from an Image.
         * 
         * @param img Image structure containing pixel data and dimensions.
         */
        Texture(const Image &img) : ptr(TextureBase::create(img)) {}

        /**
         * @brief Constructor for Texture with specified dimensions.
         * 
         * @param width Width of the texture in pixels.
         * @param height Height of the texture in pixels.
         * @param options Texture loading options.
         */
        Texture(unsigned int width, unsigned int height, TextureOptions options = TextureOptions()) : ptr(TextureBase::create(width, height, options)) {}

        /**
         * @brief Constructor for Texture from file data.
         * 
         * @param fileData Vector containing raw file data.
         * @param options Texture loading options.
         */
        Texture(const FileData &fileData, TextureOptions options = TextureOptions()) : ptr(TextureBase::create(fileData, options)) {}

        /**
         * @brief Constructor for Texture from a file path.
         * 
         * @param filepath Path to the texture file.
         * @param options Texture loading options.
         */
        Texture(const char *filepath, TextureOptions options = TextureOptions()) : ptr(TextureBase::create(filepath, options)) {}

        /**
         * @brief On-load callback proxy.
         * 
         * Allows setting a callback function that is invoked when the texture is loaded.
         * 
         * @code{.cpp}
         * Texture texture("path/to/texture.png");
         * texture.onLoad = []() {
         *    std::cout << "Texture loaded!" << std::endl;
         * };
         * @endcode
         */
        OnLoadProxy onLoad{ ptr };

        /**
         * @brief Dereference operator to access the underlying TextureBase.
         * 
         * @return Reference to the TextureBase instance.
         */
        const TextureBase& get() const { return *ptr; }

        /**
         * @brief Dereference operator to access the underlying TextureBase.
         * 
         * @return Reference to the TextureBase instance.
         */
        const TextureBase& operator*() const { return *ptr; }

        /**
         * @brief Arrow operator to access members of the underlying TextureBase.
         * 
         * @return Pointer to the TextureBase instance.
         */
        const TextureBase* operator->() const { return ptr.get(); }

        /**
         * @brief Dereference operator to access the underlying TextureBase.
         * 
         * @return Reference to the TextureBase instance.
         */
        TextureBase& get() { return *ptr; }

        /**
         * @brief Dereference operator to access the underlying TextureBase.
         * 
         * @return Reference to the TextureBase instance.
         */
        TextureBase& operator*() { return *ptr; }

        /**
         * @brief Arrow operator to access members of the underlying TextureBase.
         * 
         * @return Pointer to the TextureBase instance.
         */
        TextureBase* operator->() { return ptr.get(); }

        /**
         * @brief Boolean conversion operator.
         * 
         * @return true if the texture is valid (non-null), false otherwise.
         */
        explicit operator bool() const noexcept {
            return ptr != nullptr;
        }
};

}


namespace RaeptorCogs::Singletons {

/**
 * @brief Map of texture atlases by type.
 * 
 * Used by the TextureAtlasManager singleton to manage texture atlases.
 * @see TextureAtlasManager
 */
using TextureAtlasMap = std::map<TextureAtlasTypeKey, std::vector<std::shared_ptr<TextureAtlas>>>; // Map of texture atlases by type

/**
 * @brief Texture atlas manager singleton class.
 * 
 * Manages texture atlases and provides methods to add, remove, and retrieve atlases.
 * @see TextureAtlas
 */
class TextureAtlasManager {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Map of texture atlases.
         * @see TextureAtlasMap
         */
        TextureAtlasMap atlases;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Private constructor for TextureAtlasManager.
         * 
         * Use the SingletonAccessor to access the singleton instance.
         */
        TextureAtlasManager() = default;

        /**
         * @brief Destructor for TextureAtlasManager.
         */
        ~TextureAtlasManager();

        friend SingletonAccessor<TextureAtlasManager>;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Add a texture atlas to the manager.
         * 
         * @param atlas Shared pointer to the texture atlas to add.
         */
        void addAtlas(std::shared_ptr<TextureAtlas> atlas);

        /**
         * @brief Remove a texture atlas from the manager.
         * 
         * @param atlas Pointer to the texture atlas to remove.
         */
        void removeAtlas(const TextureAtlas *atlas);

        /**
         * @brief Sort texture atlases of a specific type.
         * 
         * @param key Type key of the texture atlases to sort.
         */
        void sort(TextureAtlasTypeKey key);

        /**
         * @brief Get a texture atlas of a specific type.
         * 
         * @param key Type key of the texture atlas to retrieve.
         * @return Shared pointer to the retrieved texture atlas.
         * 
         * @note If no suitable atlas exists returns nullptr.
         */
        std::shared_ptr<TextureAtlas> getAtlas(TextureAtlasTypeKey key);
};
}


namespace std {

/**
 * @brief Output stream operator for TextureOptions.
 * 
 * @param os Output stream.
 * @param options TextureOptions instance to output.
 * @return Reference to the output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const RaeptorCogs::TextureOptions& options) {
    return os << "TextureOptions{minFilter=" << options.minFilter
              << ", magFilter=" << options.magFilter
              << ", s_width=" << options.s_width
              << ", s_height=" << options.s_height << "}";
}

}