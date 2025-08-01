#include <RaeptorLab/text.hpp>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>


Glyph::Glyph(Text &text, char character, glm::vec2 advance) : text(&text), character(character) {
    this->setAnchor(glm::vec2(0.0f, 0.0f)); // Set default anchor to center
    setCharacter(character, advance);
}

Glyph::Glyph() : text(nullptr), character('\0') {}

void Glyph::setCharacter(char character, glm::vec2 advance) {
    this->character = character;
    this->setSize(this->text->getFont()->getGlyphSize(character));
    this->setPosition(this->text->getPosition());
    this->setRotation(this->text->getRotation());
    this->setAnchor(this->text->getAnchor());
    this->setColor(this->text->getColor());
    if (character == '\n') {
        this->setVisibility(false); // Hide glyph for newline character
    } else {
        this->setVisibility(this->text->isVisible()); // Set visibility based on text visibility
    }
    this->position += this->text->getFont()->getGlyphOffset(character) * 2.0f;
    this->position += advance;
}

void Glyph::addToRenderer(Renderer &renderer) {
    if (!isVisible()) return;

    // Add to renderer's batch
    renderer.addGraphic(this);
}

void Glyph::prepareRender(Renderer* renderer) {
    //renderer->setMode(RENDERER_MODE_2D_TEXT);
}

void Glyph::computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) {
    if (this->text->needsRebuild()) {
        this->text->rebuild(); // Ensure text is rebuilt before computing instance data
    }
    data.model = this->getModelMatrix();
    data.uvRect = this->text->getFont()->getGlyphUVRect(character);
    data.type = RENDERER_MODE_2D_TEXT;

    glm::vec3 color = this->getColor();
    data.dataOffset = instanceDataBuffer.size() / sizeof(uint8_t); // Offset into the instance data buffer
    instanceDataBuffer.insert(instanceDataBuffer.end(), reinterpret_cast<uint8_t*>(&color), reinterpret_cast<uint8_t*>(&color) + sizeof(color));
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


void Text::addToRenderer(Renderer &renderer) {
    if (!isVisible()) return;

    // Add to renderer's batch
    renderer.addGraphic(this);
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

    if (font) {
        glm::vec2 advance = glm::vec2(0.0f, 0.0f); // Initialize advance vector
        if (glyphs.size() > content.size()) {
            glyphs.resize(content.size()); // Resize glyphs vector if it has more elements than content
        }
        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];
            if (glyphs.size() < i + 1) {
                glyphs.push_back(std::make_shared<Glyph>(*this, c, advance));
                glyphs[i]->addToRenderer(*this->getRenderer()); // Add glyph to renderer
            } else {
                glyphs[i]->setCharacter(c, advance);
            }
            if (c == '\n') {
                advance.x = 0.0f; // Reset x offset for new line
                advance.y += font->getFontSize(); // Move down by font size
                continue; // Skip to the next character
            }
            advance.x += font->getGlyphXAdvance(c); // Subtract the x advance of the glyph
            advance.x += font->getGlyphSize(c).x / 2.0f; // Add the x offset of the glyph
        }
    } else {
        std::cerr << "No font set for Text object." << std::endl;
    }
}

void Text::bind() const {
    if (font) {
        font->bind();
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
    if (this->font->getID() != font.getID()) {
        this->renderer->changeGraphicPosition(this, font.getID());
        for (auto& glyph : glyphs) {
            this->renderer->changeGraphicPosition(glyph.get(), font.getID());
        }
    }
    this->font = &font;
    this->flags |= GraphicFlags::NEEDS_REBUILD;
}


Font* Text::getFont() const {
    return font;
}

GLuint Text::getID() const {
    return font ? font->getID() : 0;
}