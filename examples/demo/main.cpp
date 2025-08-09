#include <RaeptorLab/raeptorLab.hpp>
#include <RaeptorLab/memory.hpp>
#include <RaeptorLab/io/font.hpp>
#include <RaeptorLab/text.hpp>
#include <RaeptorLab/sprite.hpp>
#include <RaeptorLab/io/string.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <format>
#include <imgui.h>
#include <nfd.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>
    #include <libgen.h>
    #include <string.h>
#endif

bool setCWDToExecutableLocation() {
#if defined(_WIN32)
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return false;

    // Remove filename to get directory
    *strrchr(path, '\\') = '\0';
    return _chdir(path) == 0;

#elif defined(__linux__) || defined(__APPLE__)
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1) return false;
    path[count] = '\0';

    char *dir = dirname(path);
    return chdir(dir) == 0;

#else
    return false; // Not supported on unknown platform
#endif
}

Texture testTexture;
Font font;
std::array<Sprite, 100000> sprites;
int fontSize = 72;
Window * second_window = nullptr;

class RaeptorApplication {
    private:
        Window* window;

    public:
        RaeptorApplication();
        ~RaeptorApplication();

        int update();

        ImGuiIO& getGui();
        Window* getWindow();
};

void create_gui(Window* window) {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup ImGui style'
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window->getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
}


void load_texture_test() {
    const char* CLIENT_ID = reinterpret_cast<const char*>(load_file("protected/id.txt"));
    const char* CLIENT_SECRET = reinterpret_cast<const char*>(load_file("protected/secret.txt"));

    std::cout << "Client ID: " << CLIENT_ID << std::endl;
    std::cout << "Client Secret: " << CLIENT_SECRET << std::endl;

    httplib::Client cli("https://id.twitch.tv");


    char * randomString = new char[16];
    for (int i = 0; i < 15; ++i) {
        randomString[i] = 'a' + rand() % 26; // Generate a random lowercase letter
    }
    randomString[15] = '\0'; // Null-terminate the string

    // Twitch requires HTTPS
    cli.set_follow_location(true); // Follow redirects if needed

    // Construct query string safely
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };

    std::string body = R"({
        "client_id": ")" + std::string(CLIENT_ID) + R"(",
        "client_secret": ")" + std::string(CLIENT_SECRET) + R"(",
        "grant_type": "client_credentials"
    })";

    std::string accessToken;

    if (auto res = cli.Post("/oauth2/token", headers, body, "application/json")) {
        if (res->status == 200) {
            nlohmann::json response = nlohmann::json::parse(res->body);
            accessToken = response["access_token"];
        } else {
            std::cerr << "HTTP error: " << res->status << std::endl;
        }
    } else {
        std::cerr << "Request failed: " << res.error() << std::endl;
    }

    if (accessToken.empty()) {
        std::cerr << "Failed to obtain access token." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Access Token: " << accessToken << std::endl;

    httplib::Client igdb("https://api.igdb.com");

    igdb.set_follow_location(true); // Follow redirects if needed

    headers = {
        { "Accept", "application/json" },
        { "Client-ID", std::string(CLIENT_ID) },
        { "Authorization", std::string("Bearer ") + accessToken }
    };

    body = R"(
        fields cover.image_id; where id=332780;
    )";

    std::string cover_image_id;

    if (auto res = igdb.Post("/v4/games", headers, body, "application/json")) {
        if (res->status == 200) {
            nlohmann::json response = nlohmann::json::parse(res->body);
            cover_image_id = response[0]["cover"]["image_id"];
            std::cout << "Cover Image ID: " << cover_image_id << std::endl;
        } else {
            std::cerr << "HTTP error: " << res->status << std::endl;
        }
    } else {
        std::cerr << "Request failed: " << res.error() << std::endl;
    }

    const std::string cover_type = "cover_big"; // Replace with actual image ID from the response
    std::string cover_image_url = "https://images.igdb.com/igdb/image/upload/t_" + cover_type + "/" + cover_image_id + ".jpg";

    std::cout << "Cover Image URL: " << cover_image_url << std::endl;

    Image coverImage = load_image_from_url(cover_image_url.c_str());
    stbi_write_png("cover_image.jpg", coverImage.width, coverImage.height, coverImage.channels, coverImage.data.get(), 0);
    testTexture = Texture(coverImage);
}

RaeptorApplication::RaeptorApplication() {
    NFD_Init();
    this->window = RaeptorLab::Renderer().initialize(1280, 720, "RaeptorLab Demo");
    second_window = RaeptorLab::Renderer().createWindow(800, 600, "Second Window");
    create_gui(this->window);
    load_texture_test();
    font = Font("assets/fonts/Alef-Bold.ttf", 72);
    for (size_t i = 0; i < sprites.size(); ++i) {
        sprites[i] = Sprite(testTexture);
        sprites[i].setPosition(glm::vec2(100.0f + (i%100) * 10, 100.0f + (i/100) * 10));
        sprites[i].setSize(glm::vec2(50.0f, 50.0f));
        sprites[i].setRotation(i*1.1f);
        sprites[i].setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprites[i].setAnchor(glm::vec2(0.5f, 0.5f));
        sprites[i].setVisibility(true);
        //sprites[i].setZIndex(static_cast<float>(i)/100.0f);
    }
    for (auto& sprite : sprites) {
        sprite.addToRenderer(RaeptorLab::Renderer());
    }
    GLint maxSSBOSize;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
}

RaeptorApplication::~RaeptorApplication() {
    // Destructor implementation
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete this->window;
    delete second_window;
    glfwTerminate();
}


struct Node {
    bool expanded = false; // Tracks whether the node is expanded
    std::string type;
    Graphic *sprite;    // Example setting
};

std::vector<Node> nodes;

int RaeptorApplication::update() {
    int width, height;

    glfwGetWindowSize(window->getGLFWWindow(), &width, &height);
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Your ImGui code here
    float fps = static_cast<int>(1.0f / ImGui::GetIO().DeltaTime);
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text(("This is a sample window ( " + std::to_string(fps) + "FPS )").c_str());

    for (size_t i = 0; i < nodes.size(); ++i) {
        Node& node = nodes[i];
        ImGui::PushID(i);  // Ensure unique ID per widget set

        if (ImGui::Button("x")) {
            delete node.sprite; // Clean up the sprite
            nodes.erase(nodes.begin() + i);
            ImGui::PopID();
            break; // break to avoid invalidating indices after erase
        }
        ImGui::SameLine();
        std::string label;
        if (node.type == "sprite") {
            label = "Sprite " + std::to_string(i);
        } else if (node.type == "text") {
            label = "Text " + std::to_string(i);
        } else {
            label = "Unknown Type " + std::to_string(i);
        }
        node.expanded = ImGui::CollapsingHeader(label.c_str());

        if (node.expanded) {
            if (node.type == "sprite") {
                ImGui::Text("Sprite Properties");

                Sprite *sprite = dynamic_cast<Sprite*>(node.sprite);
                glm::vec2 rectanglePosition = sprite->getPosition();
                glm::vec2 rectangleSize = sprite->getSize();
                float rectangleRotation = sprite->getRotation();
                float rectangleZIndex = sprite->getZIndex();
                glm::vec3 rectangleColor = sprite->getColor();
                bool visible = sprite->isVisible();

                ImGui::Indent();
                ImGui::Checkbox("Visible", &visible);
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &rectanglePosition.x, 0.0f, width);
                ImGui::SliderFloat("Y", &rectanglePosition.y, 0.0f, height);
                ImGui::SliderFloat("Z Index", &rectangleZIndex, -100.0f, 100.0f);
                ImGui::SliderFloat2("Size", &rectangleSize[0], 0.0f, 2000.0f);
                ImGui::SliderAngle("Rotation", &rectangleRotation, 0.0f, 360.0f);
                ImGui::ColorEdit3("Color", &rectangleColor[0]); // Example of color edit, replace with actual color
                
                if (ImGui::Button("Open Texture")) {
                    nfdu8char_t *outPath;
                    nfdu8filteritem_t filters[1] = { { "Image Files", "png,jpg,jpeg" } };
                    nfdopendialogu8args_t args = {0};
                    args.filterList = filters;
                    args.filterCount = 1;
                    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
                        
                    if ( result == NFD_OKAY ) {
                        sprite->setTexture(ressourceManager.loadTextureFromFile(outPath));
                        NFD_FreePathU8(outPath);
                    }
                }

                ImGui::Unindent();

                sprite->setPosition(rectanglePosition);
                sprite->setSize(rectangleSize);
                sprite->setRotation(rectangleRotation);
                sprite->setColor(rectangleColor);
                sprite->setZIndex(rectangleZIndex);
                sprite->setVisibility(visible);

            } else if (node.type == "text") {
                ImGui::Text("Text Properties");

                Text *text = dynamic_cast<Text*>(node.sprite);

                char textInput[256];
                strncpy(textInput, text->getContent().c_str(), sizeof(textInput));
                ImGui::InputTextMultiline("Text Input", textInput, sizeof(textInput));
                text->setContent(textInput);

                glm::vec2 textPosition = text->getPosition();
                glm::vec2 textSize = text->getSize();
                float textRotation = text->getRotation();
                float zIndex = text->getZIndex();
                glm::vec3 textColor = text->getColor();
                bool visible = text->isVisible();
                int wordWrapType = static_cast<int>(text->getWordWrapType());
                int alignment = static_cast<int>(text->getAlignment());
                float wordWrapWidth = text->getWordWrapWidth();
                ImGui::Indent();
                ImGui::Checkbox("Visible", &visible);
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &textPosition.x, 0.0f, width);
                ImGui::SliderFloat("Y", &textPosition.y, 0.0f, height);
                ImGui::SliderFloat("Z Index", &zIndex, -100.0f, 100.0f);
                ImGui::SliderFloat2("Size", &textSize[0], 0.0f, 2000.0f);
                ImGui::SliderAngle("Rotation", &textRotation, 0.0f, 360.0f);
                ImGui::ColorEdit3("Color", &textColor[0]); // Example of color edit, replace with actual color
                ImGui::Text("Word Wrap");
                ImGui::RadioButton("None", &wordWrapType, static_cast<int>(TextWordWrap::NONE));
                ImGui::SameLine();
                ImGui::RadioButton("Word", &wordWrapType, static_cast<int>(TextWordWrap::WORD));
                ImGui::SameLine();
                ImGui::RadioButton("Character", &wordWrapType, static_cast<int>(TextWordWrap::CHARACTER));
                if (wordWrapType != static_cast<int>(TextWordWrap::NONE)) {
                    ImGui::SliderFloat("Width", &wordWrapWidth, 0.0f, 2000.0f);
                    text->setWordWrap(static_cast<TextWordWrap>(wordWrapType), wordWrapWidth);
                } else {
                    text->setWordWrap(TextWordWrap::NONE);
                }
                ImGui::Text("Alignment");
                ImGui::RadioButton("Left", &alignment, static_cast<int>(TextAlignment::LEFT));
                ImGui::SameLine();
                ImGui::RadioButton("Center", &alignment, static_cast<int>(TextAlignment::CENTER));
                ImGui::SameLine();
                ImGui::RadioButton("Right", &alignment, static_cast<int>(TextAlignment::RIGHT));
                ImGui::Unindent();
                text->setAlignment(static_cast<TextAlignment>(alignment));
                text->setPosition(textPosition);
                text->setSize(textSize);
                text->setRotation(textRotation);
                text->setColor(textColor);
                text->setZIndex(zIndex);
                text->setVisibility(visible);

                if (ImGui::Button("Open Font")) {
                    nfdu8char_t *outPath;
                    nfdu8filteritem_t filters[1] = { { "Font Files", "ttf,otf" } };
                    nfdopendialogu8args_t args = {0};
                    args.filterList = filters;
                    args.filterCount = 1;
                    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
                        
                    if ( result == NFD_OKAY ) {
                        text->setFont(ressourceManager.loadFontFromFile(outPath, fontSize));
                        NFD_FreePathU8(outPath);
                    }
                }
                ImGui::SameLine();
                ImGui::Text("Font Size");
                ImGui::SliderInt("##FontSize", &fontSize, 8, 128);
                
            }
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Sprite")) {
        Sprite* sprite = new Sprite(testTexture);
        sprite->setPosition(glm::vec2(100.0f, 100.0f));
        sprite->setSize(glm::vec2(264.0f, 352.0f));
        sprite->setRotation(0.0f);
        sprite->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprite->setAnchor(glm::vec2(0.5f, 0.5f));
        sprite->setVisibility(true);
        nodes.push_back(Node{false, "sprite", sprite});
        sprite->addToRenderer(RaeptorLab::Renderer());
    }

    if (ImGui::Button("Add Text")) {
        Text* text = new Text(font, "Sample Text");
        text->setPosition(glm::vec2(100.0f, 100.0f));
        text->setSize(text->measureTextSize());
        text->setRotation(0.0f);
        text->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        text->setAnchor(glm::vec2(0.5f, 0.5f));
        text->setVisibility(true);
        nodes.push_back(Node{false, "text", text});
        text->addToRenderer(RaeptorLab::Renderer());
    }

    ImGui::Separator();
    ImGui::End();

    ImGui::Render();

    RaeptorLab::Renderer().render(this->window);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(this->window->getGLFWWindow());
    //RaeptorLab::Renderer().render(second_window);
    //glfwSwapBuffers(second_window->getGLFWWindow());
    //this->renderer->clearBatches();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "GL error: " << err << std::endl;
    }

    if (glfwWindowShouldClose(this->window->getGLFWWindow())) {
        RaeptorLab::Renderer().clearBatches();
        for (auto& node : nodes) {
            delete node.sprite; // Clean up sprites
        }
        RaeptorLab::Renderer().destroy();
        return -1;
    }
    return 0;
}

ImGuiIO& RaeptorApplication::getGui() {
    return ImGui::GetIO();
}

Window* RaeptorApplication::getWindow() {
    return this->window;
}



int main() {
    setCWDToExecutableLocation();

    RaeptorApplication app = RaeptorApplication();

    int8_t ret_value = 0;
    while (!(ret_value = app.update()));

    return ret_value;
}
