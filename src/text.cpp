#include <RaeptorLab/text.hpp>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <RaeptorLab/io/string.hpp>


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

void Glyph::addToRenderer(Renderer &_renderer) {
    // Add to renderer's batch
    _renderer.addGraphic(this);
}

void Glyph::computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) {
    if (this->text->needsRebuild()) {
        this->text->rebuild(); // Ensure text is rebuilt before computing instance data
    }
    data.model = this->getModelMatrix();
    data.uvRect = this->text->getFont()->getGlyphUVRect(character);
    data.type = RENDERER_MODE_2D_TEXT;

    glm::vec3 col = this->getColor();
    data.dataOffset = instanceDataBuffer.size() / sizeof(uint8_t); // Offset into the instance data buffer
    instanceDataBuffer.insert(instanceDataBuffer.end(), reinterpret_cast<uint8_t*>(&col), reinterpret_cast<uint8_t*>(&col) + sizeof(col));
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


void Text::addToRenderer(Renderer &_renderer) {
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

    glm::vec2 size = font->measureTextSize(content);
    float lineHeight = font->getFontSize();

    if (font) {
        glm::vec2 advance = glm::vec2(-font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[0])) * 0.25f, lineHeight*0.75f); // Initialize advance vector
        size_t i = 0, j = 0;
        for (; content[j] != '\0'; ++i) {
            char c = content[j];
            if (glyphs.size() < i + 1) {
                glyphs.push_back(std::make_shared<Glyph>(*this, reinterpret_cast<const unsigned char*>(&content[j]), advance - size * this->anchor));
                glyphs[i]->addToRenderer(*this->getRenderer()); // Add glyph to renderer
            } else {
                glyphs[i]->setCharacter(reinterpret_cast<const unsigned char*>(&content[j]), advance - size * this->anchor);
            }
            if (c == '\n') {
                advance.x = 0.0f; // Reset x offset for new line
                advance.y += lineHeight; // Move down by line height
                j++;
                i--;
                continue; // Skip to the next character
            }
            advance.x += font->getGlyphXAdvance(reinterpret_cast<const unsigned char*>(&content[j])); // Subtract the x advance of the glyph
            advance.x += font->getGlyphSize(reinterpret_cast<const unsigned char*>(&content[j])).x / 2.0f; // Add the x offset of the glyph
            j += utf8_char_length(reinterpret_cast<const unsigned char*>(&content[j]));
        }
        glyphs.resize(i); // Resize glyphs vector to match the number of characters processed
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
    this->rendererKey = std::make_tuple(z, this->isOpaque(), this->getID());
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


Font* Text::getFont() const {
    return font;
}

GLuint Text::getID() const {
    return font ? font->getID() : 0;
}