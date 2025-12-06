// #include <vld.h>


#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Text.hpp>
#include <RaeptorCogs/Sprite.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/Camera.hpp>
#include <tests/test_mass_sprites.hpp>
#include <RaeptorCogs/Serializable.hpp>
#include <gui/menu.hpp>
#include <global.hpp>

#include <iostream>
#include <fstream>

#include <GLFW/glfw3.h>

RaeptorCogs::Window *main_window = nullptr;
int fontSize = 72;

#define GL_TEST
#define TEST_CREATE_MASS_SPRITES
//#define TEST_LOAD_MASS_SPRITES_FROM_FILES
#define TEST_LOAD_MASS_SPRITES_FROM_FILES_COUNT 999995
#define TEST_LOAD_MASS_SPRITES_FROM_FILES_SELECTABLE false

#ifdef GL_TEST

RaeptorCogs::Texture testPass1;
RaeptorCogs::Texture testPass2;
RaeptorCogs::Font testFont;
RaeptorCogs::Camera2D camera;


RaeptorCogs::Sprite2D testSprite[10];

std::vector<RaeptorCogs::Sprite2D> *sprites = nullptr;
std::vector<std::string> fileNames;

void init() {
    RaeptorCogs::Renderer().add(camera);
    camera.setZoom(1.0f);
    camera.setPosition(glm::vec2(0.0f, 0.0f));
    RaeptorCogs::Texture &testTexture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    testFont = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get_or_create("assets/fonts/Alef-Bold.ttf");
    testPass1 = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    testPass2 = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create(RaeptorCogs::UniqueKey("test"), 256, 256);

    #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
    sprites = &loadMassSprites(fileNames);
    #endif
    #ifdef TEST_CREATE_MASS_SPRITES
    sprites = &createMassSprites(testTexture, TEST_LOAD_MASS_SPRITES_FROM_FILES_COUNT, TEST_LOAD_MASS_SPRITES_FROM_FILES_SELECTABLE);
    #endif
}

glm::vec2 cameraOffset = glm::vec2(0.0f);
glm::vec2 scaleOnHover = glm::vec2(1.0f);

RaeptorCogs::Sprite2D * selectedSprite = nullptr;
RaeptorCogs::Sprite2D * hoveredSprite = nullptr;
size_t lastIndex = SIZE_MAX;
std::vector<RaeptorCogs::Sprite2D *> unhoveredSprites;

void update(RaeptorCogs::Window &main_window) {
    double deltaTime = RaeptorCogs::Time().getDeltaTime();
    float zoom = camera.getZoom();


    double speed = 1000.0 * deltaTime * (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::LEFT_SHIFT) ? 2.0 : 1.0) * zoom;

    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::UP_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::W)) {
        cameraOffset.y -= static_cast<float>(speed);
    }
    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::DOWN_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::S)) {
        cameraOffset.y += static_cast<float>(speed);
    }
    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::LEFT_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::A)) {
        cameraOffset.x -= static_cast<float>(speed);
    }
    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::RIGHT_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::D)) {
        cameraOffset.x += static_cast<float>(speed);
    }

    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::PLUS)) {
        camera.setZoom(zoom * (1.0f - 1.0f * static_cast<float>(deltaTime)));
    }
    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::MINUS)) {
        camera.setZoom(zoom * (1.0f + 1.0f * static_cast<float>(deltaTime)));
    }

    camera.setPosition(cameraOffset);

    if (selectedSprite) {
        selectedSprite->setPosition(camera.getPosition());
        selectedSprite->setScale(selectedSprite->getScale() + (glm::vec2(5.0f, 5.0f) - selectedSprite->getScale()) * static_cast<float>(deltaTime * 10.0f));
        selectedSprite->setZIndex(100.0f);
    }

    if (sprites) {
        if (RaeptorCogs::Mouse().getHoveredData() >= 1 && RaeptorCogs::Mouse().getHoveredData() <= sprites->size()) {
            size_t index = static_cast<size_t>(RaeptorCogs::Mouse().getHoveredData() - 1);
            if (&(*sprites)[index] != selectedSprite) {
                (*sprites)[index].setScale((*sprites)[index].getScale() + (glm::vec2(2.5f, 2.5f) - (*sprites)[index].getScale()) * static_cast<float>(deltaTime * 10.0f));
                (*sprites)[index].setZIndex(10.0f);
                #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
                RaeptorCogs::Texture &texture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[index].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
                texture.onLoad = [index, &texture]() {
                    static_cast<RaeptorCogs::Sprite2D*>((*sprites)[index].getChildren()[0])->setTexture(texture);
                };
                #endif
                if (hoveredSprite != &(*sprites)[index]) {
                    main_window.selectCursor(GLFW_POINTING_HAND_CURSOR);
                    if (hoveredSprite) {
                        if (lastIndex >= 0 && lastIndex < fileNames.size()) {
                            static_cast<RaeptorCogs::Sprite2D*>(hoveredSprite->getChildren()[0])->setTexture(RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.s_width = 0, .s_height = 150}));
                            RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().remove(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
                        }
                        unhoveredSprites.push_back(hoveredSprite);
                    }
                }
                hoveredSprite = &(*sprites)[index];
                lastIndex = index;
                if (std::find(unhoveredSprites.begin(), unhoveredSprites.end(), hoveredSprite) != unhoveredSprites.end()) {
                    unhoveredSprites.erase(std::remove(unhoveredSprites.begin(), unhoveredSprites.end(), &(*sprites)[index]), unhoveredSprites.end());
                }
                if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::ENTER)) {
                    selectedSprite = &(*sprites)[index];
                    hoveredSprite = nullptr;
                }
            }
        } else if (hoveredSprite) {
            #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
            if (lastIndex >= 0 && lastIndex < fileNames.size()) {
                static_cast<RaeptorCogs::Sprite2D*>(hoveredSprite->getChildren()[0])->setTexture(RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.s_width = 0, .s_height = 150}));
                RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().remove(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
            }
            RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().remove(fileNames[5].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
            #endif
            unhoveredSprites.push_back(hoveredSprite);
            hoveredSprite = nullptr;
            main_window.selectCursor(GLFW_ARROW_CURSOR);
        }
    }

    for (RaeptorCogs::Sprite2D *sprite : unhoveredSprites) {
        sprite->setScale(sprite->getScale() + (glm::vec2(1.0f, 1.0f) - sprite->getScale()) * static_cast<float>(deltaTime * 10.0f));
        if (sprite->getScale().x < 1.25f) {
            sprite->setZIndex(0.0f);
        }
        if (sprite->getScale().x < 1.01f) {
            sprite->setScale(glm::vec2(1.0f, 1.0f));
            sprite->setZIndex(0.0f);
            unhoveredSprites.erase(std::remove(unhoveredSprites.begin(), unhoveredSprites.end(), sprite), unhoveredSprites.end());
        }
    }

    createMenu(main_window.getSize());
    RaeptorCogs::Renderer().setRenderListID(0);
    RaeptorCogs::Renderer().render(main_window);
}

struct Test2 : public RaeptorCogs::Serializable {
    int x;
    float y;
    std::string z;

    Test2(int xVal, float yVal, const std::string& zVal) : x(xVal), y(yVal), z(zVal) {}

    void reflect(RaeptorCogs::Visitor& v) override {
        v.visit(x);
        v.visit(y);
        v.visit(z);
    }
};
class Test : public RaeptorCogs::Serializable {
    public:
        int a;
        float b;

        std::string c = "Hello, World!";
        Test2 d;

        glm::vec2 e;
        std::vector<int> f;

        Test(
            int aVal = 42, 
            float bVal = 3.14f, 
            std::string cVal = "Hello, World!",
            
            int d_x = 0, float d_y = 0.0f, std::string d_z = "Default",

            const glm::vec2& eVal = glm::vec2(1.0f, 2.0f), 
            const std::vector<int>& fVal = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50}
        )
            : a(aVal), b(bVal), c(cVal), d{d_x, d_y, d_z}, e(eVal), f(fVal) {}
        void reflect(RaeptorCogs::Visitor& v) override {
            v.visit(a);
            v.visit(b);
            v.visit(c);
            v.visit(d);
            v.visit(e);
            v.visit(f);
        }
};

int main() {
    // ---------- SAVE ----------
    Test save;

    for (int i = 0; i < 100; ++i) {
        save.f.push_back(i + 100);
    }
    for (int i = 0; i < 100000; ++i) {
        save.f.push_back(0);
    }

    auto data = save.dump(RaeptorCogs::SerializationFlag::COMPRESS);

    std::ofstream fout("test.bin", std::ios::binary);
    fout.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    fout.close();

    auto uncompressedData = save.dump(RaeptorCogs::SerializationFlag::NONE);

    fout.open("test_uncompressed.bin", std::ios::binary);
    fout.write(reinterpret_cast<const char*>(uncompressedData.data()), static_cast<std::streamsize>(uncompressedData.size()));
    fout.close();

    std::cout << "Original size: " << uncompressedData.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << data.size() << " bytes" << std::endl;
    std::cout << "Compression ratio: " << static_cast<float>(uncompressedData.size()) / static_cast<float>(data.size()) << std::endl;

    // ---------- LOAD ----------
    std::ifstream fin("test.bin", std::ios::binary);
    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();

    Test loaded(20, 6.28f, "Loaded String", 7, 2.71f, "Nested", glm::vec2(3.0f, 4.0f), {101, 102, 103, 104, 105});
    loaded.load(inputData, RaeptorCogs::SerializationFlag::COMPRESS);

    // Verify loaded data

    std::cout << "Loaded values: " << loaded.a << ", " << loaded.b << ", " << loaded.c << std::endl;
    std::cout << "Loaded vec2: " << loaded.e.x << ", " << loaded.e.y << std::endl;
    /*std::cout << "Loaded int array: ";
    for (int value : loaded.f) {
        std::cout << value << " ";
    }*/
    std::cout << std::endl;
    std::cout << "Nested values: " << loaded.d.x << ", " << loaded.d.y << ", " << loaded.d.z << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    std::string appName = "RaeptorCogs Demo";
    #ifdef NDEBUG
    appName += " (Release Build)";
    #else
    appName += " (Debug Build)";
    #endif
    std::cout << appName << std::endl;
    RaeptorCogs::U8String testString("Hello, RaeptorCogs!");
    RaeptorCogs::U8Char R = testString[7];
    RaeptorCogs::U8String anotherString = &R;
    std::cout << "Substring: " << anotherString.c_str() << std::endl;
    RaeptorCogs::Initialize();
    RaeptorCogs::Renderer().initialize();
    #ifndef _WIN32
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, APP_ID);
    #endif
    main_window = RaeptorCogs::Platform().createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, appName);
    main_window->setIcon({"assets/icons/raeptor-cogs-icon-16.png",
                          "assets/icons/raeptor-cogs-icon-32.png",
                          "assets/icons/raeptor-cogs-icon-48.png",
                          "assets/icons/raeptor-cogs-icon-128.png"});
    init();
    RaeptorCogs::StartLoop(update, *main_window);
    RaeptorCogs::Destroy();

    return 0;
}


#else


void update(RaeptorCogs::Window *main_window) {
    RaeptorCogs::Renderer().NewImGuiFrame();
    float fps = static_cast<int>(1.0f / ImGui::GetIO().DeltaTime);
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text(("This is a sample window ( " + std::to_string(fps) + "FPS )").c_str());
    ImGui::Separator();
    ImGui::End();

    RaeptorCogs::Renderer().setRenderListID(0);
    RaeptorCogs::Renderer().render(main_window);
}


int main() {
    RaeptorCogs::Initialize();
    RaeptorCogs::Renderer().initialize(RaeptorCogs::GraphicsBackend::Vulkan);
    #ifndef _WIN32
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, APP_ID);
    #endif
    main_window = RaeptorCogs::Renderer().createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RaeptorCogs Demo");
    main_window->setIcon({"assets/icons/raeptor-cogs-icon-16.png",
                          "assets/icons/raeptor-cogs-icon-32.png",
                          "assets/icons/raeptor-cogs-icon-48.png",
                          "assets/icons/raeptor-cogs-icon-128.png"});
    RaeptorCogs::StartLoop(update, *main_window);
    RaeptorCogs::Destroy();

    return 0;
}

#endif