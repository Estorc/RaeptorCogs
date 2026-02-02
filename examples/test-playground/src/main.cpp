// #include <vld.h>


#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/Renderer.hpp>
#include <RaeptorCogs/Core/Memory.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Graphics/Text.hpp>
#include <RaeptorCogs/Graphics/Sprite.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/Graphics/Camera.hpp>
#include <tests/test_mass_sprites.hpp>
#include <RaeptorCogs/Core/Serializable.hpp>
#include <gui/menu.hpp>
#include <global.hpp>

#include <iostream>
#include <fstream>

#ifdef __linux__
#ifdef RAEPTOR_HAS_WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

RaeptorCogs::Window *main_window = nullptr;
int fontSize = 72;

//#define GL_TEST
//#define TEST_CREATE_MASS_SPRITES
//#define TEST_CREATE_MASS_SPRITES_COUNT 999995
//#define TEST_CREATE_MASS_SPRITES_SELECTABLE true
#define TEST_LOAD_MASS_SPRITES_FROM_FILES
#define TEST_SELECTION_HOVERING
#define CAMERA_ZOOM_INITIAL 1.0f
#define CAMERA_ZOOM_STEP 1.0f

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
    camera.setZoom(CAMERA_ZOOM_INITIAL);
    camera.setPosition(glm::vec2(0.0f, 0.0f));
    RaeptorCogs::Texture &testTexture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    testFont = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get_or_create("assets/fonts/Alef-Bold.ttf");
    testPass1 = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    testPass2 = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create(RaeptorCogs::UniqueKey("test"), 256, 256);

    #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
    sprites = &loadMassSprites(fileNames);
    #endif
    #ifdef TEST_CREATE_MASS_SPRITES
    sprites = &createMassSprites(testTexture, TEST_CREATE_MASS_SPRITES_COUNT, TEST_CREATE_MASS_SPRITES_SELECTABLE);
    #endif
}

glm::vec2 cameraOffset = glm::vec2(0.0f);
glm::vec2 scaleOnHover = glm::vec2(1.0f);

RaeptorCogs::Sprite2D * selectedSprite = nullptr;
RaeptorCogs::Sprite2D * hoveredSprite = nullptr;
glm::vec2 selectedSpriteOriginalPosition = glm::vec2(0.0f);
glm::vec2 selectedSpriteOffset = glm::vec2(0.0f);
glm::vec2 mouseOffsetOnSelect = glm::vec2(0.0f);
size_t lastIndex = SIZE_MAX;
std::vector<RaeptorCogs::Sprite2D *> unhoveredSprites;


void closeSelectedSprite() {
    if (selectedSprite) {
        static_cast<RaeptorCogs::Sprite2D*>(selectedSprite->getChildren()[0])->setTexture(RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.s_width = 0, .s_height = 200}));
        RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().remove(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
        selectedSprite->setScale(glm::vec2(1.0f, 1.0f));
        selectedSprite->setZIndex(0.0f);
        selectedSprite->setPosition(selectedSpriteOriginalPosition);
        selectedSprite = nullptr;
    }
}

int mousePressedMode = 0;

void update(RaeptorCogs::Window &main_window) {
    double deltaTime = RaeptorCogs::Time().getDeltaTime();
    float zoom = camera.getZoom();


    double speed = 1000.0 * deltaTime * (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::LEFT_SHIFT) ? 2.0 : 1.0) * zoom;

    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::PLUS)) {
        camera.setZoom(zoom * (1.0f - CAMERA_ZOOM_STEP * static_cast<float>(deltaTime)));
    }
    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::MINUS)) {
        camera.setZoom(zoom * (1.0f + CAMERA_ZOOM_STEP * static_cast<float>(deltaTime)));
    }

    if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::ESCAPE)) {
        closeSelectedSprite();
    }

    camera.setPosition(cameraOffset);

    #ifdef TEST_SELECTION_HOVERING

    bool isSelectedSpriteHovered = false;

    if (sprites) {
        if (RaeptorCogs::Mouse().getHoveredData() >= 1 && RaeptorCogs::Mouse().getHoveredData() <= sprites->size()) {
            size_t index = static_cast<size_t>(RaeptorCogs::Mouse().getHoveredData() - 1);

            if (&(*sprites)[index] == selectedSprite) {
                isSelectedSpriteHovered = true;
                if (hoveredSprite) {
                    unhoveredSprites.push_back(hoveredSprite);
                    hoveredSprite = nullptr;
                    main_window.selectCursor(GLFW_ARROW_CURSOR);
                }
            } else {
                (*sprites)[index].setScale((*sprites)[index].getScale() + (glm::vec2(1.5f,1.5f) - (*sprites)[index].getScale()) * static_cast<float>(deltaTime * 10.0f));
                (*sprites)[index].setZIndex(10.0f);
                #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
                /*RaeptorCogs::Texture &texture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[index].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
                texture.onLoad = [index, &texture]() {
                    static_cast<RaeptorCogs::Sprite2D*>((*sprites)[index].getChildren()[0])->setTexture(texture);
                };*/
                #endif
                if (hoveredSprite != &(*sprites)[index]) {
                    main_window.selectCursor(GLFW_POINTING_HAND_CURSOR);
                    if (hoveredSprite) {
                        unhoveredSprites.push_back(hoveredSprite);
                    }
                }
                hoveredSprite = &(*sprites)[index];
                if (std::find(unhoveredSprites.begin(), unhoveredSprites.end(), hoveredSprite) != unhoveredSprites.end()) {
                    unhoveredSprites.erase(std::remove(unhoveredSprites.begin(), unhoveredSprites.end(), &(*sprites)[index]), unhoveredSprites.end());
                }
                if (RaeptorCogs::Mouse().isButtonJustReleased(RaeptorCogs::MouseButton::LEFT) && 
                    (glm::lessThan(glm::abs(RaeptorCogs::Mouse().getPosition() - mouseOffsetOnSelect), glm::vec2(10.0f)).x && glm::lessThan(glm::abs(RaeptorCogs::Mouse().getPosition() - mouseOffsetOnSelect), glm::vec2(10.0f)).y)) {

                    RaeptorCogs::Texture &texture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[index].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
                    texture.onLoad = [index, &texture]() {
                        static_cast<RaeptorCogs::Sprite2D*>((*sprites)[index].getChildren()[0])->setTexture(texture);
                    };

                    closeSelectedSprite();

                    lastIndex = index;

                    selectedSprite = &(*sprites)[index];
                    selectedSprite->setZIndex(100.0f);
                    selectedSprite->setScale(glm::vec2(4.0f, 4.0f));
                    selectedSpriteOriginalPosition = selectedSprite->getPosition();
                    selectedSpriteOffset = glm::vec2(0.0f);
                    hoveredSprite = nullptr;
                }
            }
        } else if (hoveredSprite) {
            #ifdef TEST_LOAD_MASS_SPRITES_FROM_FILES
            if (lastIndex >= 0 && lastIndex < fileNames.size()) {
                //static_cast<RaeptorCogs::Sprite2D*>(hoveredSprite->getChildren()[0])->setTexture(RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.s_width = 0, .s_height = 150}));
                //RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().remove(fileNames[lastIndex].c_str(), RaeptorCogs::TextureOptions{.priority = 10});
            }
            #endif
            unhoveredSprites.push_back(hoveredSprite);
            hoveredSprite = nullptr;
            main_window.selectCursor(GLFW_ARROW_CURSOR);
        }
    }

    if (!selectedSprite || !isSelectedSpriteHovered) {
        if (RaeptorCogs::Mouse().isButtonJustPressed(RaeptorCogs::MouseButton::LEFT)) {
            mousePressedMode = 1;
            mouseOffsetOnSelect = RaeptorCogs::Mouse().getPosition();
        }
    }

    if (selectedSprite) {
        if (RaeptorCogs::Mouse().getScrollY() != 0.0f) {
            float scrollY = RaeptorCogs::Mouse().getScrollY();
            selectedSprite->setScale(selectedSprite->getScale() * glm::vec2(1.0f + scrollY * 0.1f, 1.0f + scrollY * 0.1f));
            // Make offset scale with zoom and mouse position
            selectedSpriteOffset = (selectedSpriteOffset - (RaeptorCogs::Mouse().getPosition() - glm::vec2(main_window.getSize().x / 2.0f, main_window.getSize().y / 2.0f)) * camera.getZoom()) * (1.0f + scrollY * 0.1f) + (RaeptorCogs::Mouse().getPosition() - glm::vec2(main_window.getSize().x / 2.0f, main_window.getSize().y / 2.0f)) * camera.getZoom();
        }
        if (isSelectedSpriteHovered) {
            if (RaeptorCogs::Mouse().isButtonJustPressed(RaeptorCogs::MouseButton::LEFT)) {
                mousePressedMode = 2;
            }
        }

        if (mousePressedMode == 2) {
            if (RaeptorCogs::Mouse().isButtonPressed(RaeptorCogs::MouseButton::LEFT)) {
                selectedSpriteOffset += RaeptorCogs::Mouse().getDeltaPosition();
            }
            if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::LEFT_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::A)) {
                selectedSpriteOffset.x += static_cast<float>(speed);
            }
            if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::RIGHT_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::D)) {
                selectedSpriteOffset.x -= static_cast<float>(speed);
            }
            if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::UP_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::W)) {
                selectedSpriteOffset.y += static_cast<float>(speed);
            }
            if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::DOWN_ARROW) || RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::S)) {
                selectedSpriteOffset.y -= static_cast<float>(speed);
            }
        }

        selectedSprite->setPosition(camera.getPosition() + selectedSpriteOffset);
    }

    for (size_t i = 0; i < unhoveredSprites.size(); ++i) {
        RaeptorCogs::Sprite2D *sprite = unhoveredSprites[i];
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
    #else
    mousePressedMode = 1;
    #endif

    if (mousePressedMode == 1) {
        if (RaeptorCogs::Mouse().isButtonPressed(RaeptorCogs::MouseButton::LEFT)) {
            cameraOffset.y -= static_cast<float>(RaeptorCogs::Mouse().getDeltaPosition().y) * zoom;
            cameraOffset.x -= static_cast<float>(RaeptorCogs::Mouse().getDeltaPosition().x) * zoom;
        }
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
    RaeptorCogs::Renderer().initialize(RaeptorCogs::GraphicsBackend::GL);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    main_window = RaeptorCogs::Platform().createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, appName);
    glfwSwapInterval(1);
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


#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

#ifdef _WIN32
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

void FeedImGuiMouse() {
    ImGuiIO& io = ImGui::GetIO();

    HWND hwnd = glfwGetWin32Window(main_window->getGLFWWindow());

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hwnd, &p);

    io.AddMousePosEvent((float)p.x, (float)p.y);
}
#elif __linux__

#ifdef RAEPTOR_HAS_WAYLAND

#include <wayland-client-core.h>
#include <wayland-client.h>

static struct wl_compositor* compositor = NULL;

static void registry_global(void *data,
                            struct wl_registry *registry,
                            uint32_t name,
                            const char *interface,
                            uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name,
                                                    &wl_compositor_interface,
                                                    4)); // version 4 is safe
    }
}

static void registry_remove(void *data,
                            struct wl_registry *registry,
                            uint32_t name)
{
    // Usually nothing to do
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_remove
};

struct wl_surface* surface;

void FeedSurfaceIfChanged()
{
    static ImVec2 last_pos  = {-1, -1};
    static ImVec2 last_size = {-1, -1};

    ImVec2 pos  = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();

    if (pos.x != last_pos.x || pos.y != last_pos.y ||
        size.x != last_size.x || size.y != last_size.y)
    {
        last_pos  = pos;
        last_size = size;

        struct wl_region* region = wl_compositor_create_region(compositor);
        wl_region_add(region, pos.x, pos.y, size.x, size.y);
        wl_surface_set_input_region(surface, region);
        wl_region_destroy(region);
        wl_surface_commit(surface);
    }
}

#endif

#else
void FeedImGuiMouse() {
    // No-op on non-Windows platforms
}
#endif

void update(RaeptorCogs::Window &main_window) {
    
    RaeptorCogs::Renderer().CreateImGuiFrame();
    float fps = static_cast<int>(1.0f / ImGui::GetIO().DeltaTime);
    ImGui::Begin("Hello, ImGui!");
    ImGui::TextUnformatted(("This is a sample window ( " + std::to_string(fps) + "FPS )").c_str());
    ImGui::Separator();

    #ifdef __linux__
    FeedSurfaceIfChanged();
    #elif _Win32
    FeedImGuiMouse();
    if (ImGui::GetIO().WantCaptureMouse) {
        glfwSetWindowAttrib(main_window.getGLFWWindow(), GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
    } else {
        glfwSetWindowAttrib(main_window.getGLFWWindow(), GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
    }
    #endif

    ImGui::End();

    RaeptorCogs::Renderer().setRenderListID(0);
    RaeptorCogs::Renderer().render(main_window);
}


int main() {
    RaeptorCogs::Initialize();
    RaeptorCogs::Renderer().initialize(RaeptorCogs::GraphicsBackend::Vulkan);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, APP_ID);
    main_window = RaeptorCogs::Platform().createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RaeptorCogs Demo");
    main_window->setIcon({"assets/icons/raeptor-cogs-icon-16.png",
                          "assets/icons/raeptor-cogs-icon-32.png",
                          "assets/icons/raeptor-cogs-icon-48.png",
                          "assets/icons/raeptor-cogs-icon-128.png"});
    #ifdef __linux__
    #ifdef RAEPTOR_HAS_WAYLAND
    surface = glfwGetWaylandWindow(main_window->getGLFWWindow());
    struct wl_display* display = glfwGetWaylandDisplay();
    struct wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);
    if (compositor) {
        std::cout << "Wayland compositor obtained successfully." << std::endl;
    } else {
        std::cerr << "Failed to get Wayland compositor" << std::endl;
        return -1;
    }
    #endif
    #endif
    RaeptorCogs::StartLoop(update, *main_window);
    RaeptorCogs::Destroy();

    return 0;
}

#endif

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main();
}
#endif