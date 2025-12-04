#include <RaeptorCogs/Text.hpp>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
namespace RaeptorCogs {

Glyph::Glyph(Text2D &text, const U8Char &character, glm::vec2 advance) : text(&text), character(character) {
    this->setAnchor(glm::vec2(0.0f, 0.0f)); // Set default anchor to center
    setCharacter(character, advance);
}

Glyph::Glyph() : text(nullptr), character(nullptr) {}

void Glyph::setCharacter(const U8Char &_character, glm::vec2 advance) {
    advance *= (this->text->getTextSize() / NORMAL_FONT_SIZE);
    this->character = _character;
    this->setSize(this->text->getFont()->getGlyphSize(_character) * (this->text->getTextSize() / NORMAL_FONT_SIZE));
    this->setVisibility(this->text->isVisible());
    this->setPosition(glm::vec2(0.0f, 0.0f));
    this->setRotation(0.0f);
    this->setZIndex(this->text->getZIndex());
    this->setAnchor(glm::vec2(0.0f, 0.0f)); // Reset anchor to top-left

    glm::vec2 glyphOffset = this->text->getFont()->getGlyphOffset(_character) * (this->text->getTextSize() / NORMAL_FONT_SIZE);
    
    this->setPosition(this->getPosition() + glyphOffset * 2.0f + advance);
}

bool Glyph::computeInstanceData(GAPI::Common::InstanceAllocator &instanceAllocator, ComputeInstanceDataMode mode) {
    GAPI::Common::GraphicBatchHandler &batchHandler = this->getBatchHandler();

    if (this->text->isTextDirty()) {
        this->text->rebuildText(); // Ensure text is rebuilt before computing instance data
    }

    if (mode == ComputeInstanceDataMode::FORCE_REBUILD) {
        instanceAllocator.allocate(batchHandler, 4); // RGB color + smoothness
    }

    auto& staticDataBuffer = instanceAllocator.getStaticInstanceData(batchHandler.staticDataCursor);
    auto* dynamicDataBuffer = instanceAllocator.getDynamicInstanceData(batchHandler.dynamicDataCursor);

    if (this->isDataDirty() || mode == ComputeInstanceDataMode::FORCE_REBUILD) {

        // Static instance data

        staticDataBuffer.model = this->getModelMatrix();
        staticDataBuffer.uvRect = this->text->getFont()->getGlyphUVRect(character);
        staticDataBuffer.type = this->isVisible() ? RENDERER_MODE_2D_TEXT : RENDERER_MODE_DEFAULT;
        staticDataBuffer.readingMaskID = this->getReadingMaskID();
        staticDataBuffer.writingMaskID = this->getWritingMaskID();
        if (mode == ComputeInstanceDataMode::FORCE_REBUILD) {
            staticDataBuffer.dataOffset = batchHandler.dynamicDataCursor; // Offset into the instance data buffer
        }

        // Dynamic instance data

        glm::vec3 color = this->getGlobalColor();
        float smoothness = 0.2f * (NORMAL_FONT_SIZE / this->text->getTextSize());
        dynamicDataBuffer[0] = color[0];
        dynamicDataBuffer[1] = color[1];
        dynamicDataBuffer[2] = color[2];
        dynamicDataBuffer[3] = std::min(smoothness, 0.5f);
    }

    if (this->isDataDirty()) {
        this->setDataDirty(false);
        return true;
    }
    return false;

}

void Glyph::bind() const {
    this->text->getFont()->bind();
}

GLuint Glyph::getID() const {
    return this->text->getFont()->getID();
}

Text2D::Text2D(Font &font, const U8String &content) : font(font), content(content) {
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY);
    FlagSet<GraphicFlags>::setFlag(GraphicFlags::NO_BATCHING);
    this->setLocalMatrixDirty(true);
    this->setVisibility(true);
}

Text2D::~Text2D() {
    glyphs.clear();
   // this->setRenderer(nullptr);
}


void Text2D::setRenderer(Singletons::Renderer *renderer) {
    // Add to renderer's batch
    Graphic2D::setRenderer(renderer);
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY);
    this->rebuildText(); // Ensure text is rebuilt before adding to renderer
}

void Text2D::rebuildText() {
    if (!this->getRenderer()) {
        return;
    }
    if (!FlagSet<TextFlags>::hasFlag(TextFlags::TEXT_DIRTY)) {
        return;
    }
    // Set the rebuild flag
    FlagSet<TextFlags>::clearFlag(TextFlags::TEXT_DIRTY);

    // Rebuild glyphs based on content and font

    glm::vec2 size = this->measureTextSize();
    float lineHeight = font->getFontSize();

    for (auto& glyph : glyphs) {
        this->removeChild(glyph.get());
    }


    if (font) {
        glm::vec2 advance = glm::vec2(0.0f, lineHeight * 0.75f); // Initialize advance vector
        size_t charCount = 0, lastSpaceCharCount = 0;
        U8CharIterator lastSpaceIt = content.end();

        float alignOffset = 0.0f;
        if (this->alignment == TextAlignment::CENTER) {
            alignOffset = (size.x - measureLineWidth(content)) / 2.0f;
        } else if (this->alignment == TextAlignment::RIGHT) {
            alignOffset = size.x - measureLineWidth(content);
        }

        for (auto it = content.begin(); it != content.end(); ++it, ++charCount) {
            U8Char c = *it;
            if (glyphs.size() < charCount + 1) {
                glyphs.push_back(std::make_shared<Glyph>(*this, c, advance - glm::vec2(size.x - alignOffset*2.0f, size.y) * this->getAnchor()));
                this->getRenderer()->add(*glyphs[charCount].get());
            } else {
                glyphs[charCount]->setCharacter(c, advance - glm::vec2(size.x - alignOffset*2.0f, size.y) * this->getAnchor());
            }
            if (c == "\n") {
                advance.x = 0.0f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height
                charCount--;

                c = *(it+1); // Move to the character after the space

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&c)) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&c);
                }
                continue; // Skip to the next character
            }
            if (c == " ") {
                lastSpaceIt = it; // Remember the last space iterator
                lastSpaceCharCount = charCount;
            }
            advance.x += font->getGlyphXAdvance(c); // Subtract the x advance of the glyph
            //advance.x += font->getGlyphSize(reinterpret_cast<const unsigned char*>(&content[i])).x / 2.0f; // Add the x offset of the glyph

            if (this->wordWrapType == TextWordWrap::CHARACTER && advance.x > this->wordWrapWidth) {
                // If character wrapping is enabled and the width exceeds the limit, move to the next line
                advance.x = 0.0f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&c)) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&c);
                }
            }
            if (this->wordWrapType == TextWordWrap::WORD && advance.x > this->wordWrapWidth && lastSpaceIt != content.end()) {
                // If word wrapping is enabled and the width exceeds the limit, move to the next line
                advance.x = 0; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height
                it = lastSpaceIt; // Reset iterator to last space
                charCount = lastSpaceCharCount; // Reset character count to the last space
                lastSpaceIt = content.end();

                c = *(it+1); // Move to the character after the space

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&c)) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&c);
                }
                continue;
            }
        }
        glyphs.resize(charCount); // Resize glyphs vector to match the number of characters processed
    } else {
        std::cerr << "No font set for Text object." << std::endl;
    }

    for (auto& glyph : glyphs) {
        this->addChild(glyph.get());
    }
}

void Text2D::bind() const {
    if (font) {
        font->bind();
    }
}

void Text2D::setZIndex(float z) {
    TransformableGraphic2D::setZIndex(z);
    //this->rendererKey = std::make_tuple(z, this->isOpaque(), this->getID());
    for (auto& glyph : glyphs) {
        glyph->setZIndex(z);
    }
}

void Text2D::setContent(const U8String &content) {
    if (content == this->content) return; // No change in content
    this->content = content;
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY);
    this->rebuildText(); // Rebuild glyphs based on new content
}

const U8String& Text2D::getContent() const {
    return content;
}

void Text2D::setFont(Font &font) {
    bool needChangeGraphicPosition = (this->font && this->font->getID() != font->getID());
    this->font = font;
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY);
    if (needChangeGraphicPosition) {
        for (auto& glyph : glyphs) {
            glyph->updatePositionInRenderLists();
        }
    }
}

void Text2D::setWordWrap(TextWordWrap wrap, float width) {
    this->wordWrapType = wrap;
    this->wordWrapWidth = width;
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY); // Mark as needing rebuild
    this->rebuildText(); // Rebuild text with new word wrap settings
}

void Text2D::setAlignment(TextAlignment align) {
    this->alignment = align;
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY); // Mark as needing rebuild
    this->rebuildText(); // Rebuild text with new alignment settings
}

void Text2D::setTextSize(float size) {
    this->textSize = size;
    FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY); // Mark as needing rebuild
    this->rebuildText(); // Rebuild text with new text size
}

void Text2D::setTextDirty(bool dirty) {
    if (dirty) {
        FlagSet<TextFlags>::setFlag(TextFlags::TEXT_DIRTY);
    } else {
        FlagSet<TextFlags>::clearFlag(TextFlags::TEXT_DIRTY);
    }
}



Font Text2D::getFont() const {
    return font;
}

GLuint Text2D::getID() const {
    return font ? font->getID() : 0;
}

TextWordWrap Text2D::getWordWrapType() const {
    return wordWrapType;
}

float Text2D::getWordWrapWidth() const {
    return wordWrapWidth;
}

TextAlignment Text2D::getAlignment() const {
    return this->alignment;
}

float Text2D::getTextSize() const {
    return textSize;
}

bool Text2D::isTextDirty() const {
    return FlagSet<TextFlags>::hasFlag(TextFlags::TEXT_DIRTY);
}




glm::vec2 Text2D::measureTextSize() const {
    glm::vec2 size(0.0f, 0.0f);
    float currentLineWidth = 0.0f;
    float lineHeight = this->font->getFontSize();
    float lastSpaceWidth = 0.0f; // Width of the last space character
    U8CharIterator lastSpaceIt = content.end();
    for (auto it = content.begin(); it != content.end(); ++it) {
        U8Char c = *it;
        if (c == "\n") {
            size.x = std::max(size.x, currentLineWidth);
            size.y += lineHeight;
            currentLineWidth = 0.0f; // Reset for new line
            continue;
        }
        if (c == " ") {
            lastSpaceIt = it; // Remember the last space index
            lastSpaceWidth = currentLineWidth; // Get the width of the space character
        }
        GlyphData* glyph = this->font->getGlyph(c);
        if (glyph) {
            currentLineWidth += glyph->getXAdvance();
        }
        if (this->wordWrapType == TextWordWrap::CHARACTER && currentLineWidth > this->wordWrapWidth) {
            // If character wrapping is enabled and the width exceeds the limit, move to the next line
            size.x = std::max(size.x, currentLineWidth); // Reset x offset for new line
            size.y += lineHeight; // Move down by line height
            currentLineWidth = 0.0f; // Reset for new line
        }
        if (this->wordWrapType == TextWordWrap::WORD && currentLineWidth > this->wordWrapWidth && lastSpaceIt != content.end()) {
            // If word wrapping is enabled and the width exceeds the limit, move to the next line
            currentLineWidth = lastSpaceWidth; // Reset to the width of the last space
            size.x = std::max(size.x, currentLineWidth); // Reset x offset for new line
            size.y += lineHeight; // Move down by line height
            it = lastSpaceIt; // Move back to the last space
            lastSpaceIt = content.end(); // Reset last space index
            currentLineWidth = 0.0f; // Reset for new line
            continue;
        }
    }
    size.x = std::max(size.x, currentLineWidth);
    size.y += lineHeight;
    return size;
}

float Text2D::measureLineWidth(const U8String& text) const {
    float width = 0.0f;
    float lastSpaceWidth = 0.0f; // Width of the last space character
    for (auto c : text) {
        if (c == "\n") {
            break;
        }
        if (c == " ") {
            lastSpaceWidth = width; // Get the width of the space character
        }
        GlyphData* glyph = this->font->getGlyph(c);
        if (glyph) {
            width += glyph->getXAdvance();
        }
        if (this->wordWrapType == TextWordWrap::CHARACTER && width > this->wordWrapWidth) {
            // If character wrapping is enabled and the width exceeds the limit, move to the next line
            break;
        }
        if (this->wordWrapType == TextWordWrap::WORD && width > this->wordWrapWidth && lastSpaceWidth != 0.0f) {
            // If word wrapping is enabled and the width exceeds the limit, move to the next line
            width = lastSpaceWidth; // Reset to the width of the last space
            break;
        }
    }
    return width;
}

bool Text2D::isVisible() const {
    return RenderableGraphic2D::isVisible() && font && font->isLoaded();
}

}