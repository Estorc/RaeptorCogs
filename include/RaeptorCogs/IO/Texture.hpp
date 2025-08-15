#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <vector>
#include <stack>
#include <RaeptorCogs/IO/Images.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_rect_pack.h>

namespace RaeptorCogs {

/** TODO:
 * - Constant atlas loader
 * - Dynamic atlas
 * - Texture packing
 */

enum class TextureAtlasFlags : uint32_t {
    NONE = 0,
    NEEDS_REBUILD = 1 << 0, // Flag to indicate if the atlas needs to be rebuilt
};

template<>
struct EnableBitmaskOperators<TextureAtlasFlags> {
    static constexpr bool enable = true;
};

#define COMMON_ATLAS_WIDTH  1024
#define COMMON_ATLAS_HEIGHT 1024
#define ATLAS_PADDING 1

class Texture;

class TextureAtlas {
    private:
        glm::ivec2 size; // Size of the atlas
        std::vector<Texture> textures; // List of textures in the atlas
        GLuint textureID; // Texture ID in OpenGL
        stbrp_context ctx; // STB rect pack context for packing textures
        std::vector<stbrp_node> nodes; // Nodes for packing
        TextureAtlasFlags flags; // Flags for the atlas (e.g., if it needs to be rebuilt)

    public:
        TextureAtlas();
        TextureAtlas(glm::ivec2 size);

        void bind() const;
        void unbind() const;

        void uploadTexture(GLuint x, GLuint y, GLuint width, GLuint height, const void *data);
        bool tryAddTexture(Texture &texture, const Image &img);
        void removeTexture(const Texture &texture);

        GLuint getID() const;
        int getWidth() const;
        int getHeight() const;
};

class Texture {
    private:
        TextureAtlas *atlas; // Reference to the texture atlas this texture belongs to
        glm::vec4 rect; // Rectangle in the atlas
        glm::vec4 uvRect; // UV rectangle in the atlas
        bool opaque;

    public:
        Texture() : atlas(nullptr), opaque(false) {}
        Texture(const Image &img);
        Texture(const FileData &fileData);
        Texture(const char *filepath);

        void bind() const;

        bool isOpaque() const;

        void setAtlas(TextureAtlas *atlas);
        void setRect(const glm::vec4 &rect);
        void setUVRect(const glm::vec4 &uv);

        glm::vec4 getRect();
        glm::vec4 getUVRect();
        TextureAtlas *getAtlas() const;
        GLuint getID() const;
};

}

namespace RaeptorCogs::Singletons {
class TextureAtlasManager {
    private:
        std::stack<TextureAtlas> atlases; // Stack of texture atlases

    public:
        TextureAtlasManager();

        void addAtlas(const TextureAtlas &atlas);
        void removeAtlas(const TextureAtlas &atlas);
        TextureAtlas* getAtlas();
};
}