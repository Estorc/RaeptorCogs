#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Sprite.hpp>
#include <RaeptorCogs/Window.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/Text.hpp>
#include <RaeptorCogs/Camera.hpp>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

RaeptorCogs::Camera2D camera;

RaeptorCogs::Sprite2D raeptorLogoSprite;

struct RaeptorParticle {
    RaeptorCogs::Sprite2D sprite;
    float speed;
};

std::array<RaeptorParticle, 10000> raeptorParticles;

RaeptorCogs::Text2D raeptorText;
RaeptorCogs::Text2D creditsText;

float lastTime = 0.0f;

std::string GetFpsString() {
    std::ostringstream fps;
    fps << std::fixed << std::setprecision(2) << 1.0f / RaeptorCogs::Time().getDeltaTime();
    return fps.str();
}

void UpdateText() {
    std::ostringstream fps;
    fps << std::fixed << std::setprecision(2) << 1.0f / RaeptorCogs::Time().getDeltaTime();
    raeptorText.setContent("Welcome to RæptorCogs!\nC++ Graphics/Game Engine Framework (" + GetFpsString() + " fps)");
}

void Initialize() {
    RaeptorCogs::Renderer().add(camera);

    RaeptorCogs::Font &defaultFont = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get_or_create("assets/fonts/Alef-Bold.ttf");
    defaultFont.onLoad = [&]() {
        std::cout << "Default font loaded!" << std::endl;
        raeptorText = RaeptorCogs::Text2D(defaultFont);
        raeptorText.setZIndex(10.0f);
        raeptorText.setTextSize(48.0f);
        raeptorText.setPosition({0.0f, -200.0f});
        raeptorText.setAnchor({0.5f, 0.5f});
        raeptorText.setAlignment(RaeptorCogs::TextAlignment::CENTER);
        RaeptorCogs::Renderer().add(raeptorText);
        creditsText = RaeptorCogs::Text2D(defaultFont, "RæptorCogs Demo - © RÆPTOR\nDeveloped by Estorc\n2025 © MIT License");
        creditsText.setZIndex(10.0f);
        creditsText.setTextSize(24.0f);
        creditsText.setPosition({0.0f, 250.0f});
        creditsText.setAnchor({0.5f, 0.5f});
        creditsText.setAlignment(RaeptorCogs::TextAlignment::CENTER);
        RaeptorCogs::Renderer().add(creditsText);
    };

    RaeptorCogs::Texture& logoTexture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    logoTexture.onLoad = [&]() {
        std::cout << "RaeptorCogs logo texture loaded!" << std::endl;
        raeptorLogoSprite.setTexture(logoTexture);
        raeptorLogoSprite.setPosition({0.0f, 0.0f});
        raeptorLogoSprite.setAnchor({0.5f, 0.5f});
        raeptorLogoSprite.setSize({200.0f, 200.0f});
        raeptorLogoSprite.setZIndex(5.0f);
        RaeptorCogs::Renderer().add(raeptorLogoSprite);
        for (size_t i = 0; i < raeptorParticles.size(); ++i) {
            raeptorParticles[i].sprite.setTexture(logoTexture);
            float scale = RaeptorCogs::Random().getFloat(0.1f, 1.0f);
            raeptorParticles[i].sprite.setScale({scale, scale});
            raeptorParticles[i].sprite.setSize({20.0f, 20.0f});
            raeptorParticles[i].sprite.setPosition({0.0f, INFINITY});
            raeptorParticles[i].sprite.setAnchor({0.5f, 0.5f});
            raeptorParticles[i].sprite.setColor({RaeptorCogs::Random().getFloat(0.0f, 1.0f), RaeptorCogs::Random().getFloat(0.0f, 1.0f), RaeptorCogs::Random().getFloat(0.0f, 1.0f)});
            RaeptorCogs::Renderer().add(raeptorParticles[i].sprite);
            raeptorParticles[i].speed = 50.0f + RaeptorCogs::Random().getFloat(0.0f, 150.0f);
        }
    };
}

void Update(RaeptorCogs::Window& mainWindow) {

    raeptorText.setRotation(sinf(RaeptorCogs::Time().getTime()) * 0.1f);
    if (lastTime + 1.0f < RaeptorCogs::Time().getTime() && raeptorText.isVisible()) {
        lastTime = RaeptorCogs::Time().getTime();
        UpdateText();
        mainWindow.setTitle("RæptorCogs Demo - " + GetFpsString() + " FPS");
    }

    glm::vec2 frameSize = glm::vec2(mainWindow.getSize()) * 1.8f;
    camera.setZoom(sinf(RaeptorCogs::Time().getTime() * 3.0f) * 0.2f + 1.5f);
    raeptorLogoSprite.setRotation(sinf(RaeptorCogs::Time().getTime()) * 0.5f);
    raeptorLogoSprite.setScale({1.0f + 0.1f * sinf(RaeptorCogs::Time().getTime() * 2.0f),
                                 1.0f + 0.1f * sinf(RaeptorCogs::Time().getTime() * 2.0f)});
    for (size_t i = 0; i < raeptorParticles.size(); ++i) {
        glm::vec2 pos = raeptorParticles[i].sprite.getPosition();
        pos.y += raeptorParticles[i].speed * RaeptorCogs::Time().getDeltaTime();
        if (pos.y > frameSize.y / 2.0f) {
            pos.y = -frameSize.y / 2.0f;
            pos.x = RaeptorCogs::Random().getFloat(0.0f, frameSize.x) - frameSize.x / 2.0f;
        }
        raeptorParticles[i].sprite.setPosition(pos);
        raeptorParticles[i].sprite.setRotation(raeptorParticles[i].sprite.getRotation() + raeptorParticles[i].speed / 30.0f * RaeptorCogs::Time().getDeltaTime());
    }
    RaeptorCogs::Renderer().render(mainWindow);
}

int main() {
    RaeptorCogs::Initialize();
    RaeptorCogs::Renderer().initialize();
    Initialize();
    RaeptorCogs::Window& mainWindow = *RaeptorCogs::Platform().createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RaeptorCogs Demo");
    mainWindow.setIcon({
                        "assets/icons/raeptor-cogs-icon-16.png",
                        "assets/icons/raeptor-cogs-icon-32.png",
                        "assets/icons/raeptor-cogs-icon-48.png",
                        "assets/icons/raeptor-cogs-icon-128.png"
                        });
    RaeptorCogs::StartLoop(Update, mainWindow);
    RaeptorCogs::Destroy();
    return 0;
}