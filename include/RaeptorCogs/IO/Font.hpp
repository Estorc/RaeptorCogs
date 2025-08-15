#pragma once
#include <RaeptorCogs/IO/FileIO.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <string>
namespace RaeptorCogs {
class GlyphData {
    private:
        float u0, v0, u1, v1; // texture coordinates
        float x_offset, y_offset; // in pixels
        float width, height;      // in pixels
        float x_advance;          // cursor movement
    public:
        GlyphData() : u0(0), v0(0), u1(0), v1(0),
                      x_offset(0), y_offset(0), width(0), height(0), x_advance(0) {}
        GlyphData(float u0, float v0, float u1, float v1,
                  float x_offset, float y_offset, float width, float height, float x_advance)
            : u0(u0), v0(v0), u1(u1), v1(v1),
              x_offset(x_offset), y_offset(y_offset), width(width), height(height), x_advance(x_advance) {}

        glm::vec4 getUVRect() const;
        glm::vec2 getOffset() const;
        glm::vec2 getSize() const;
        float getXAdvance() const;
};

class Font {
    private:
        GLuint textureID;           // OpenGL texture
        std::unordered_map<uint32_t, std::unique_ptr<GlyphData>> glyphs; // UTF-8 support
        float font_size;
        bool is_dynamic;          // true = loaded from TTF
    public:
        Font(const FileData& ttf_buffer, int fontSize);
        Font(const char* fontPath, int fontSize);
        Font() {};
        ~Font();

        // Move constructor
        Font(Font&& other) noexcept = default;

        // Move assignment
        Font& operator=(Font&& other) noexcept = default;

        // Delete copy constructor and assignment
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        void uploadTexture(unsigned char* data, int width, int height, const unsigned char* ttf_buffer);
        void bind() const;

        GLuint getID() const;
        GlyphData* getGlyph(const unsigned char *character) const;
        glm::vec4 getGlyphUVRect(const unsigned char *character) const;
        glm::vec2 getGlyphOffset(const unsigned char *character) const;
        glm::vec2 getGlyphSize(const unsigned char *character) const;
        float getGlyphXAdvance(const unsigned char *character) const;
        float getFontSize() const;
};

}