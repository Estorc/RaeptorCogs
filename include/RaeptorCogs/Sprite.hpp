#pragma once
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/Flags.hpp>
namespace RaeptorCogs::Singletons {
class Renderer;
}
namespace RaeptorCogs {

class Sprite : public TransformableGraphic2D {
    private:
        Texture *texture;

    public:
        Sprite(Texture &texture);
        Sprite() : texture(nullptr) {}

        void addToRenderer(Singletons::Renderer &renderer);
        virtual void computeInstanceData(InstanceData &data, std::vector<float> &instanceDataBuffer) override;
        virtual void bind() const override;
        virtual GLuint getID() const override;
        virtual bool isOpaque() const override;

        void setTexture(Texture &texture);

        Texture* getTexture() const;
};

}