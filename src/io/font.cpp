#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <io/font.hpp>
#include <io/string.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>


glm::vec4 GlyphData::getUVRect() const {
    return glm::vec4(u0, v0, u1 - u0, v1 - v0);
}

glm::vec2 GlyphData::getOffset() const {
    return glm::vec2(x_offset, y_offset);
}

glm::vec2 GlyphData::getSize() const {
    return glm::vec2(width, height);
}

float GlyphData::getXAdvance() const {
    return x_advance;
}

Font::Font(const char *fontPath, int fontSize) : textureID(0), font_size(fontSize), is_dynamic(true) {
    unsigned char* ttf_buffer = load_file(fontPath);
    if (!ttf_buffer) {
        std::cerr << "Failed to load font file: " << fontPath << std::endl;
        return;
    }

    int atlas_width = 2048, atlas_height = 2048;
    unsigned char* data = (unsigned char*)malloc(atlas_width * atlas_height);
    if (!data) {
        std::cerr << "Failed to allocate memory for font texture." << std::endl;
        delete[] ttf_buffer;
        return;
    }

    uploadTexture(data, atlas_width, atlas_height, ttf_buffer);
    std::cout << "Font loaded successfully: " << fontPath << std::endl;
}


Font::~Font() {
    if (!textureID) return;
    for (auto& glyph : glyphs) {
        glyph.reset(); // Clean up GlyphData pointers
    }
    //glDeleteTextures(1, &textureID);
}

void Font::uploadTexture(unsigned char* data, int width, int height, unsigned char* ttf_buffer) {

    stbtt_pack_context ctx;
    if (stbtt_PackBegin(&ctx, data, width, height, 0, 1, NULL) == 0) {
        std::cerr << "Failed to initialize font packing context." << std::endl;
        delete[] ttf_buffer;
        free(data);
        return; // Return empty font
    }

    stbtt_packedchar packed_glyphs[128];
    stbtt_PackSetOversampling(&ctx, 2, 2); // better quality
    stbtt_PackFontRange(&ctx, ttf_buffer, 0, this->font_size, 0, 128, packed_glyphs); // ASCII 0–127
    for (int i = 0; i < 128; ++i) {
        this->glyphs[i] = std::make_unique<GlyphData>(
            packed_glyphs[i].x0 / (float)width,
            packed_glyphs[i].y0 / (float)height,
            packed_glyphs[i].x1 / (float)width,
            packed_glyphs[i].y1 / (float)height,
            (packed_glyphs[i].xoff)/2.0f,
            (packed_glyphs[i].yoff)/2.0f,
            (packed_glyphs[i].x1 - packed_glyphs[i].x0)/2.0f,
            (packed_glyphs[i].y1 - packed_glyphs[i].y0)/2.0f,
            (packed_glyphs[i].xadvance)/2.0f
        );
    }
    stbtt_PackEnd(&ctx);

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    free(data);
    delete[] ttf_buffer;
}

void Font::bind() const {
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

GLuint Font::getID() const {
    return this->textureID;
}

GlyphData* Font::getGlyph(char character) const {
    if (character < 0 || character >= 128) return nullptr;
    return glyphs[character].get();
}

glm::vec4 Font::getGlyphUVRect(char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec4(0.0f);
    return glyph->getUVRect();
}

glm::vec2 Font::getGlyphOffset(char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getOffset();
}

glm::vec2 Font::getGlyphSize(char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getSize();
}

float Font::getGlyphXAdvance(char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return 0.0f;
    return glyph->getXAdvance();
}

float Font::getFontSize() const {
    return font_size;
}