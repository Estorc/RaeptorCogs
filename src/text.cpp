#include <RaeptorCogs/Text.hpp>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <RaeptorCogs/IO/String.hpp>
namespace RaeptorCogs {

Glyph::Glyph(Text &text, const unsigned char *character, glm::vec2 advance) : text(&text), character(character) {
    this->setAnchor(glm::vec2(0.0f, 0.0f)); // Set default anchor to center
    setCharacter(character, advance);
}

Glyph::Glyph() : text(nullptr), character(nullptr) {}

void Glyph::setCharacter(const unsigned char *_character, glm::vec2 advance) {
    this->character = _character;
    this->setSize(this->text->getFont()->getGlyphSize(_character));
    this->setVisibility(this->text->isVisible());
    this->setPosition(this->text->getPosition());
    this->setRotation(this->text->getRotation());
    this->setZIndex(this->text->getZIndex());
    this->setAnchor(glm::vec2(0.0f, 0.0f)); // Reset anchor to top-left
    this->setColor(this->text->getColor());

    // Apply rotation to the glyph offset and advance
    float sinRot = sin(this->text->getRotation());
    float cosRot = cos(this->text->getRotation());
    
    // Rotate glyph offset
    glm::vec2 rotatedOffset = glm::vec2(
        this->text->getFont()->getGlyphOffset(_character).x * cosRot - this->text->getFont()->getGlyphOffset(_character).y * sinRot,
        this->text->getFont()->getGlyphOffset(_character).x * sinRot + this->text->getFont()->getGlyphOffset(_character).y * cosRot
    );
    
    // Rotate advance
    glm::vec2 rotatedAdvance = glm::vec2(
        advance.x * cosRot - advance.y * sinRot,
        advance.x * sinRot + advance.y * cosRot
    );
    
    this->position += rotatedOffset * 2.0f;
    this->position += rotatedAdvance;
}

void Glyph::addToRenderer(Singletons::Renderer &_renderer) {
    // Add to renderer's batch
    _renderer.addGraphic(this);
}

void Glyph::computeInstanceData(InstanceData &data, std::vector<float> &instanceDataBuffer) {
    if (this->text->needsRebuild()) {
        this->text->rebuild(); // Ensure text is rebuilt before computing instance data
    }
    data.model = this->getModelMatrix();
    data.uvRect = this->text->getFont()->getGlyphUVRect(character);
    data.type = RENDERER_MODE_2D_TEXT;

    glm::vec3 color = this->getColor();
    data.dataOffset = instanceDataBuffer.size(); // Offset into the instance data buffer
    instanceDataBuffer.insert(instanceDataBuffer.end(), reinterpret_cast<float*>(&color), reinterpret_cast<float*>(&color) + sizeof(color) / sizeof(float));
}

void Glyph::bind() const {
    this->text->getFont()->bind();
}

GLuint Glyph::getID() const {
    return this->text->getFont()->getID();
}

Text::Text(Font &font, const std::string &content) : font(&font), content(content) {
    this->flags |= GraphicFlags::NEEDS_REBUILD;
    this->setVisibility(true);
}

Text::~Text() {
    glyphs.clear();
}


void Text::addToRenderer(Singletons::Renderer &_renderer) {
    // Add to renderer's batch
    _renderer.addGraphic(this);
    this->rebuild(); // Ensure text is rebuilt before adding to renderer
}

void Text::rebuild() {
    if (!(this->flags & GraphicFlags::NEEDS_REBUILD)) {
        return;
    }
    // Set the rebuild flag
    this->flags &= ~GraphicFlags::NEEDS_REBUILD;

    // Rebuild the model matrix
    this->modelMatrix = glm::mat4(1.0f);
    this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(position, 0.0f));
    this->modelMatrix = glm::rotate(this->modelMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(size, 1.0f));

    // Rebuild glyphs based on content and font

    glm::vec2 size = this->measureTextSize();
    float lineHeight = font->getFontSize();

    if (font) {
        glm::vec2 advance = glm::vec2(-font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f, lineHeight*0.75f); // Initialize advance vector
        size_t i = 0, charCount = 0, lastSpaceIndex = 0, lastSpaceCharCount = 0;

        float alignOffset = 0.0f;
        if (this->alignment == TextAlignment::CENTER) {
            alignOffset = (size.x - measureLineWidth(&content[i])) / 2.0f;
        } else if (this->alignment == TextAlignment::RIGHT) {
            alignOffset = size.x - measureLineWidth(&content[i]);
        }

        for (; content[i] != '\0'; ++charCount) {
            char c = content[i];
            if (glyphs.size() < charCount + 1) {
                glyphs.push_back(std::make_shared<Glyph>(*this, reinterpret_cast<const unsigned char*>(&content[i]), advance - glm::vec2(size.x - alignOffset*2.0f, size.y) * this->anchor));
                glyphs[charCount]->addToRenderer(*this->getRenderer()); // Add glyph to renderer
            } else {
                glyphs[charCount]->setCharacter(reinterpret_cast<const unsigned char*>(&content[i]), advance - glm::vec2(size.x - alignOffset*2.0f, size.y) * this->anchor);
            }
            if (c == '\n') {
                advance.x = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height
                i++;
                charCount--;

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&content[i])) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&content[i]);
                }
                continue; // Skip to the next character
            }
            if (c == ' ') {
                lastSpaceIndex = i; // Remember the last space index
                lastSpaceCharCount = charCount;
            }
            advance.x += font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[i])); // Subtract the x advance of the glyph
            advance.x += font->getGlyphSize(reinterpret_cast<const unsigned char*>(&content[i])).x / 2.0f; // Add the x offset of the glyph

            if (this->wordWrapType == TextWordWrap::CHARACTER && advance.x > this->wordWrapWidth) {
                // If character wrapping is enabled and the width exceeds the limit, move to the next line
                advance.x = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&content[i])) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&content[i]);
                }
            }
            if (this->wordWrapType == TextWordWrap::WORD && advance.x > this->wordWrapWidth && lastSpaceIndex) {
                // If word wrapping is enabled and the width exceeds the limit, move to the next line
                advance.x = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height
                i = lastSpaceIndex + 1; // Move back to the last space
                charCount = lastSpaceCharCount; // Reset character count to the last space
                lastSpaceIndex = 0; // Reset last space index

                if (this->alignment == TextAlignment::CENTER) {
                    alignOffset = (size.x - measureLineWidth(&content[i])) / 2.0f;
                } else if (this->alignment == TextAlignment::RIGHT) {
                    alignOffset = size.x - measureLineWidth(&content[i]);
                }
                continue;
            }

            i += utf8_char_length(reinterpret_cast<const unsigned char*>(&content[i]));
        }
        glyphs.resize(charCount); // Resize glyphs vector to match the number of characters processed
    } else {
        std::cerr << "No font set for Text object." << std::endl;
    }
}

void Text::bind() const {
    if (font) {
        font->bind();
    }
}

void Text::setZIndex(float z) {
    this->zIndex = z;
    //this->rendererKey = std::make_tuple(z, this->isOpaque(), this->getID());
    this->renderer->changeGraphicPosition(this);
    for (auto& glyph : glyphs) {
        glyph->setZIndex(z);
    }
}

void Text::setContent(const std::string &content) {
    if (content == this->content) return; // No change in content
    this->content = content;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
    this->rebuild(); // Rebuild glyphs based on new content
}

const std::string& Text::getContent() const {
    return content;
}

void Text::setFont(Font &font) {
    bool needChangeGraphicPosition = (this->font && this->font->getID() != font.getID());
    this->font = &font;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
    if (needChangeGraphicPosition) {
        this->renderer->changeGraphicPosition(this);
        for (auto& glyph : glyphs) {
            this->renderer->changeGraphicPosition(glyph.get());
        }
    }
}

void Text::setWordWrap(TextWordWrap wrap, float width) {
    this->wordWrapType = wrap;
    this->wordWrapWidth = width;
    this->flags |= GraphicFlags::NEEDS_REBUILD; // Mark as needing rebuild
    this->rebuild(); // Rebuild text with new word wrap settings
}

void Text::setAlignment(TextAlignment align) {
    this->alignment = align;
    this->flags |= GraphicFlags::NEEDS_REBUILD; // Mark as needing rebuild
    this->rebuild(); // Rebuild text with new alignment settings
}


Font* Text::getFont() const {
    return font;
}

GLuint Text::getID() const {
    return font ? font->getID() : 0;
}

TextWordWrap Text::getWordWrapType() const {
    return wordWrapType;
}

float Text::getWordWrapWidth() const {
    return wordWrapWidth;
}

TextAlignment Text::getAlignment() const {
    return this->alignment;
}




glm::vec2 Text::measureTextSize() const {
    glm::vec2 size(0.0f, 0.0f);
    float currentLineWidth = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f;
    float lineHeight = this->font->getFontSize();
    float lastSpaceWidth = 0.0f; // Width of the last space character
    size_t i = 0, lastSpaceIndex = 0;
    for (; content[i] != '\0';) {
        unsigned char c = static_cast<unsigned char>(content[i]);
        if (c == '\n') {
            size.x = std::max(size.x, currentLineWidth);
            size.y += lineHeight;
            currentLineWidth = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset for new line
            i++;
            continue;
        }
        if (c == ' ') {
            lastSpaceIndex = i; // Remember the last space index
            lastSpaceWidth = currentLineWidth; // Get the width of the space character
        }
        GlyphData* glyph = this->font->getGlyph(reinterpret_cast<const unsigned char*>(&content[i]));
        if (glyph) {
            currentLineWidth += glyph->getXAdvance();
            currentLineWidth += glyph->getSize().x / 2.0f;
        }
        if (this->wordWrapType == TextWordWrap::CHARACTER && currentLineWidth > this->wordWrapWidth) {
            // If character wrapping is enabled and the width exceeds the limit, move to the next line
            size.x = std::max(size.x, currentLineWidth); // Reset x offset for new line
            size.y += lineHeight; // Move down by line height
            currentLineWidth = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset for new line
        }
        if (this->wordWrapType == TextWordWrap::WORD && currentLineWidth > this->wordWrapWidth && lastSpaceIndex) {
            // If word wrapping is enabled and the width exceeds the limit, move to the next line
            currentLineWidth = lastSpaceWidth; // Reset to the width of the last space
            size.x = std::max(size.x, currentLineWidth); // Reset x offset for new line
            size.y += lineHeight; // Move down by line height
            i = lastSpaceIndex + 1; // Move back to the last space
            lastSpaceIndex = 0; // Reset last space index
            currentLineWidth = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f; // Reset for new line
            continue;
        }
        i += utf8_char_length(reinterpret_cast<const unsigned char*>(&content[i]));
    }
    size.x = std::max(size.x, currentLineWidth);
    size.y += lineHeight;
    return size;
}

float Text::measureLineWidth(const std::string& text) const {
    float width = -font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f;
    float lastSpaceWidth = 0.0f; // Width of the last space character
    size_t i = 0;
    for (; text[i] != '\0';) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == '\n') {
            break;
        }
        if (c == ' ') {
            lastSpaceWidth = width; // Get the width of the space character
        }
        GlyphData* glyph = this->font->getGlyph(reinterpret_cast<const unsigned char*>(&text[i]));
        if (glyph) {
            width += glyph->getXAdvance();
            width += glyph->getSize().x / 2.0f;
        }
        if (this->wordWrapType == TextWordWrap::CHARACTER && width > this->wordWrapWidth) {
            // If character wrapping is enabled and the width exceeds the limit, move to the next line
            break;
        }
        if (this->wordWrapType == TextWordWrap::WORD && width > this->wordWrapWidth && lastSpaceWidth) {
            // If word wrapping is enabled and the width exceeds the limit, move to the next line
            width = lastSpaceWidth; // Reset to the width of the last space
            break;
        }
        i += utf8_char_length(reinterpret_cast<const unsigned char*>(&text[i]));
    }
    return width;
}

}