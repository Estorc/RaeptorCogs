#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <RaeptorLab/io/font.hpp>
#include <RaeptorLab/io/string.hpp>
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
        if (glyph.second)
            glyph.second.reset(); // Clean up GlyphData pointers
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

    // Support for larger character set (Basic Multilingual Plane)
    const int num_chars = 1024; // Increase to cover more Unicode points
    stbtt_packedchar* packed_glyphs = new stbtt_packedchar[num_chars];
    
    stbtt_PackSetOversampling(&ctx, 2, 2); // better quality
    
    // Pack characters in the Basic Multilingual Plane (BMP)
    stbtt_PackFontRange(&ctx, ttf_buffer, 0, this->font_size, 0, num_chars, packed_glyphs);
    
    // Create glyph data for each character
    for (int i = 0; i < num_chars; ++i) {
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
    
    delete[] packed_glyphs;
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

GlyphData* Font::getGlyph(const unsigned char* character) const {
    size_t char_length = utf8_char_length(character);
    if (char_length == 0) return nullptr; // Invalid character
    uint32_t codepoint = 0;
    if (char_length == 1) {
        // ASCII (0xxxxxxx)
        codepoint = static_cast<unsigned char>(character[0]);
    } else if (char_length == 2) {
        codepoint = static_cast<unsigned char>(character[0]) & 0x1F; // 00011111
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[1]) & 0x3F; // 00111111
    } else if (char_length == 3) {
        codepoint = static_cast<unsigned char>(character[0]) & 0x0F; // 00001111
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[1]) & 0x3F;
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[2]) & 0x3F;
    } else if (char_length == 4) {
        codepoint = static_cast<unsigned char>(character[0]) & 0x07; // 00000111
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[1]) & 0x3F;
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[2]) & 0x3F;
        codepoint <<= 6;
        codepoint |= static_cast<unsigned char>(character[3]) & 0x3F;
    }

    return glyphs.find(codepoint) != glyphs.end() ? glyphs.at(codepoint).get() : nullptr;
}

glm::vec4 Font::getGlyphUVRect(const unsigned char* character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec4(0.0f);
    return glyph->getUVRect();
}

glm::vec2 Font::getGlyphOffset(const unsigned char* character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getOffset();
}

glm::vec2 Font::getGlyphSize(const unsigned char* character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getSize();
}

float Font::getGlyphXAdvance(const unsigned char* character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return 0.0f;
    return glyph->getXAdvance();
}

float Font::getFontSize() const {
    return font_size;
}