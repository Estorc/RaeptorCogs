#include <tests/test_mass_sprites.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <array>

std::vector<RaeptorCogs::Sprite> sprites;

void createMassSprites(RaeptorCogs::Texture& texture, int count) {
    sprites.resize(count);
    for (size_t i = 0; i < count; ++i) {
        sprites[i] = RaeptorCogs::Sprite(texture);
        sprites[i].setPosition(glm::vec2(100.0f + (i%100) * 10, 100.0f + (i/100) * 10));
        sprites[i].setSize(glm::vec2(50.0f, 50.0f));
        sprites[i].setRotation(i*1.1f);
        sprites[i].setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprites[i].setAnchor(glm::vec2(0.5f, 0.5f));
        sprites[i].setVisibility(true);
        sprites[i].addToRenderer(RaeptorCogs::Renderer());
        //sprites[i].setZIndex(static_cast<float>(i)/100.0f);
    }
}