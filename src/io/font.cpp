#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>

#include <RaeptorCogs/External/stb/stb.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <RaeptorCogs/External/glad/glad.hpp>

namespace RaeptorCogs {

void GlyphData::setUVRect(float u0, float v0, float u1, float v1) {
    this->uv = glm::vec4(u0, v0, u1 - u0, v1 - v0);
}

glm::vec4 GlyphData::getUVRect() const {
    return this->uv;
}

glm::vec2 GlyphData::getOffset() const {
    return this->offset;
}

glm::vec2 GlyphData::getSize() const {
    return this->size;
}

float GlyphData::getXAdvance() const {
    return x_advance;
}

std::shared_ptr<FontBase> FontBase::create(const FileData& ttf_buffer, FontOptions options) {
    auto font = std::shared_ptr<FontBase>(new FontBase());
    font->textureID = 0;
    font->font_size = options.size;
    font->font_type = options.type;
    int priority = options.priority;

    RaeptorCogs::ResourceWorker().addJob([font, ttf_buffer, priority]() {
        if (ttf_buffer.empty()) {
            std::cerr << "Failed to load font file." << std::endl;
            return;
        }

        size_t atlas_width = 2048, atlas_height = 2048;
        std::vector<unsigned char> data(atlas_width * atlas_height, 0);
        if (data.empty()) {
            std::cerr << "Failed to allocate memory for font texture." << std::endl;
            return;
        }

        const int num_chars = 1024; // Increase to cover more Unicode points

        font->GenerateSDFAtlas(ttf_buffer.data(), data.data(), atlas_width, atlas_height, NORMAL_FONT_SIZE, num_chars);
        RaeptorCogs::MainWorker().addJob([font, data, atlas_width, atlas_height]() {
            font->uploadTexture(data.data(), atlas_width, atlas_height);
            #ifndef NDEBUG
            std::cout << "Font loaded successfully." << std::endl;
            #endif
            if (font->onLoad_) font->onLoad_();
        }, priority);
    }, priority);

    return font;
}

std::shared_ptr<FontBase> FontBase::create(const std::filesystem::path& fontPath, FontOptions options) {
    return create(LoadFile(fontPath), options);
}

FontBase::~FontBase() {
    if (!textureID) return;
    for (auto& glyph : glyphs) {
        if (glyph.second)
            glyph.second.reset(); // Clean up GlyphData pointers
    }
    //glDeleteTextures(1, &textureID);
}


void FontBase::GenerateSDFAtlas(const unsigned char* ttf_buffer, unsigned char* data, size_t atlas_w, size_t atlas_h, float font_px, size_t num_chars) {

    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0))) {
        std::cerr << "Font init failed\n";
        return;
    }

    // Set up rect packer
    stbrp_context ctx{};
    std::vector<stbrp_node> nodes(atlas_w);
    stbrp_init_target(&ctx, static_cast<int>(atlas_w), static_cast<int>(atlas_h), nodes.data(), static_cast<int>(atlas_w));

    if (!data) {
        std::cerr << "Failed to allocate memory for font atlas." << std::endl;
        return;
    }

    float scale = stbtt_ScaleForPixelHeight(&font, font_px);

    for (unsigned int cp = 0; cp < num_chars; ++cp) { // skip control chars
        int w,h,xoff,yoff;
        unsigned char* sdf = stbtt_GetCodepointSDF(
            &font, scale, static_cast<int>(cp),
            8, 128, 64.0f,
            &w, &h, &xoff, &yoff);

        stbrp_rect rect{};
        rect.w = w + FONT_PADDING * 2;
        rect.h = h + FONT_PADDING * 2;
 
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, static_cast<int>(cp), &advance, &lsb);

        this->glyphs[cp] = std::make_unique<GlyphData>(
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            static_cast<float>(xoff)/2.0f,
            static_cast<float>(yoff)/2.0f,
            static_cast<float>(w),
            static_cast<float>(h),
            static_cast<float>(advance) * scale
        );

        if (!sdf) continue;

        if (!stbrp_pack_rects(&ctx, &rect, 1) || !rect.was_packed) {
            free(sdf);
            continue;
        }

        this->glyphs[cp]->setUVRect(
            static_cast<float>(rect.x + FONT_PADDING) / static_cast<float>(atlas_w),
            static_cast<float>(rect.y + FONT_PADDING) / static_cast<float>(atlas_h),
            static_cast<float>(rect.x + w - FONT_PADDING * 2) / static_cast<float>(atlas_w),
            static_cast<float>(rect.y + h - FONT_PADDING * 2) / static_cast<float>(atlas_h)
        );

        // Blit into atlas
        for (size_t row = 0; row < static_cast<size_t>(h); ++row) {
            memcpy(data + (static_cast<size_t>(rect.y) + row) * atlas_w + static_cast<size_t>(rect.x),
                sdf + row * static_cast<size_t>(w), static_cast<size_t>(w));
        }

        free(sdf);
    }

}

void FontBase::GenerateBMPAtlas(const unsigned char* ttf_buffer, unsigned char* data, size_t atlas_w, size_t atlas_h, float font_px, size_t num_chars) {
    (void)font_px;
    stbtt_pack_context ctx;
    if (stbtt_PackBegin(&ctx, data, static_cast<int>(atlas_w), static_cast<int>(atlas_h), 0, 1, NULL) == 0) {
        std::cerr << "Failed to initialize font packing context." << std::endl;
        free(data);
        return; // Return empty font
    }

    // Support for larger character set (Basic Multilingual Plane)
    stbtt_packedchar* packed_glyphs = new stbtt_packedchar[num_chars];
    
    stbtt_PackSetOversampling(&ctx, 2, 2); // better quality
    
    // Pack characters in the Basic Multilingual Plane (BMP)
    stbtt_PackFontRange(&ctx, ttf_buffer, 0, this->font_size, 0, static_cast<int>(num_chars), packed_glyphs);
    
    // Create glyph data for each character
    for (unsigned int i = 0; i < static_cast<unsigned int>(num_chars); ++i) {
        this->glyphs[i] = std::make_unique<GlyphData>(
            packed_glyphs[i].x0 / static_cast<float>(atlas_w),
            packed_glyphs[i].y0 / static_cast<float>(atlas_h),
            packed_glyphs[i].x1 / static_cast<float>(atlas_w),
            packed_glyphs[i].y1 / static_cast<float>(atlas_h),
            static_cast<float>(packed_glyphs[i].xoff)/2.0f,
            static_cast<float>(packed_glyphs[i].yoff)/2.0f,
            (packed_glyphs[i].x1 - packed_glyphs[i].x0)/2.0f,
            (packed_glyphs[i].y1 - packed_glyphs[i].y0)/2.0f,
            packed_glyphs[i].xadvance
        );
    }
    
    delete[] packed_glyphs;
    stbtt_PackEnd(&ctx);
}

void FontBase::uploadTexture(const unsigned char* data, size_t width, size_t height) {

    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    #ifdef __EMSCRIPTEN__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    #else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RED, GL_UNSIGNED_BYTE, data);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void FontBase::bind() const {
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

GLuint FontBase::getID() const {
    return this->textureID;
}

GlyphData* FontBase::getGlyph(const U8Char character) const {
    uint32_t codepoint = character.codepoint();
    return glyphs.find(codepoint) != glyphs.end() ? glyphs.at(codepoint).get() : nullptr;
}

glm::vec4 FontBase::getGlyphUVRect(const U8Char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec4(0.0f);
    return glyph->getUVRect();
}

glm::vec2 FontBase::getGlyphOffset(const U8Char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getOffset();
}

glm::vec2 FontBase::getGlyphSize(const U8Char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return glm::vec2(0.0f);
    return glyph->getSize();
}

float FontBase::getGlyphXAdvance(const U8Char character) const {
    GlyphData* glyph = getGlyph(character);
    if (!glyph) return 0.0f;
    return glyph->getXAdvance();
}

float FontBase::getFontSize() const {
    return font_size;
}

bool FontBase::isLoaded() const {
    return this->textureID != 0;
}

}