#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Text.hpp>
#include <RaeptorCogs/Sprite.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <tests/test_mass_sprites.hpp>
#include <gui/menu.hpp>
#include <global.hpp>

#include <iostream>

RaeptorCogs::Texture testTexture;
RaeptorCogs::Font testFont;
int fontSize = 72;
RaeptorCogs::Window *main_window = nullptr;

void init() {
    testTexture = RaeptorCogs::Texture("assets/textures/texture.png");
    testFont = RaeptorCogs::Font("assets/fonts/Alef-Bold.ttf", 72);
    createMassSprites(testTexture, 10000);
}

void update() {
    createMenu(main_window->getSize());
    RaeptorCogs::Renderer().render(main_window);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "GL error: " << err << std::endl;
    }
}

int main() {
    RaeptorCogs::Initialize();
    main_window = RaeptorCogs::Renderer().initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "RaeptorCogs Demo");
    init();
    RaeptorCogs::StartLoop(update, main_window);
    RaeptorCogs::Destroy();

    return 0;
}
