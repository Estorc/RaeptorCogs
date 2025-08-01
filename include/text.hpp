#pragma once
#include <main.hpp>
#include <io/font.hpp>
#include <renderer.hpp>
#include <graphic.hpp>
#include <flags.hpp>

class Renderer;
class Text;
class Glyph;

class Glyph : public TransformableGraphic2D {
    private:
        Text *text; // Pointer to the text this glyph belongs to
        char character; // The character represented by this glyph

    public:
        Glyph(Text &text, char character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));
        Glyph();

        void setCharacter(char character, glm::vec2 advance = glm::vec2(0.0f, 0.0f));

        void addToRenderer(Renderer &renderer);
        virtual void prepareRender(Renderer* renderer) override;
        void computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) override;
        void bind() const override;
        GLuint getID() const override;
};

class Text : public TransformableGraphic2D {
    private:
        Font *font;
        std::vector<std::shared_ptr<Glyph>> glyphs; // Glyph data for each character in the text

        std::string content;
    public:
        Text(Font &font, const std::string &content);
        Text(Font &font) : Text(font, "") {}
        Text() : font(nullptr) {}
        virtual ~Text() override;

        void addToRenderer(Renderer &renderer);
        virtual void rebuild() override;
        virtual void bind() const override;
        virtual GLuint getID() const override;

        void setContent(const std::string &content);
        void setFont(Font &font);

        const std::string& getContent() const;
        Font* getFont() const;
};