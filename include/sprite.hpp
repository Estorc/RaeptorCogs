#pragma once
#include <main.hpp>
#include <texture.hpp>
#include <renderer.hpp>
#include <graphic.hpp>
#include <flags.hpp>

class Renderer;

class Sprite : public TransformableGraphic2D {
    private:
        Texture *texture;

    public:
        Sprite(Texture &texture);
        Sprite() : texture(nullptr) {}

        void addToRenderer(Renderer &renderer);
        virtual void prepareRender(Renderer* renderer) override;
        virtual void computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) override;
        virtual void bind() const override;
        virtual GLuint getID() const override;

        void setTexture(Texture &texture);

        Texture* getTexture() const;
};