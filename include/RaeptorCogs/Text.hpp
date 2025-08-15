#pragma once
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <string>

namespace RaeptorCogs::Singletons {
class Renderer;
}
namespace RaeptorCogs {

class Text;
class Glyph;

class Glyph : public TransformableGraphic2D {
    private:
        Text *text; // Pointer to the text this glyph belongs to
        const unsigned char *character; // The character represented by this glyph

    public:
        Glyph(Text &text, const unsigned char *character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));
        Glyph();

        void setCharacter(const unsigned char *character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));

        void addToRenderer(Singletons::Renderer &renderer);
        void computeInstanceData(InstanceData &data, std::vector<float> &instanceDataBuffer) override;
        void bind() const override;
        GLuint getID() const override;
        virtual bool isOpaque() const override { return false; }
};


enum class TextWordWrap {
    NONE,       // No word wrapping
    WORD,       // Wrap at word boundaries
    CHARACTER   // Wrap at character boundaries
};

enum class TextAlignment {
    LEFT,
    CENTER,
    RIGHT
};

class Text : public TransformableGraphic2D {
    private:
        Font *font;
        std::vector<std::shared_ptr<Glyph>> glyphs; // Glyph data for each character in the text

        std::string content;
        float wordWrapWidth = 0.0f;
        TextWordWrap wordWrapType = TextWordWrap::NONE; // Word wrapping mode
        TextAlignment alignment = TextAlignment::LEFT; // Text alignment
    public:
        Text(Font &font, const std::string &content);
        Text(Font &font) : Text(font, "") {}
        Text() : font(nullptr) {}
        virtual ~Text() override;

        void addToRenderer(Singletons::Renderer &renderer);
        virtual void rebuild() override;
        virtual void bind() const override;
        virtual GLuint getID() const override;
        virtual bool isOpaque() const override { return false; }
        virtual void setZIndex(float z) override;

        void setContent(const std::string &content);
        void setFont(Font &font);
        void setWordWrap(TextWordWrap wrap, float width = 0.0f);
        void setAlignment(TextAlignment align);

        const std::string& getContent() const;
        Font* getFont() const;
        TextWordWrap getWordWrapType() const;
        float getWordWrapWidth() const;
        TextAlignment getAlignment() const;
        
        glm::vec2 measureTextSize() const;
        float measureLineWidth(const std::string& text) const;
};

}