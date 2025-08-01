#pragma once
#include <RaeptorLab/texture.hpp>
#include <RaeptorLab/renderer.hpp>
#include <RaeptorLab/graphic.hpp>
#include <RaeptorLab/flags.hpp>

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