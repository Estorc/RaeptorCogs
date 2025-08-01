#include <RaeptorLab/memory.hpp>
#include <RaeptorLab/io/font.hpp>
#include <RaeptorLab/text.hpp>
#include <RaeptorLab/renderer.hpp>
#include <RaeptorLab/sprite.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <nfd.h>

const char* CLIENT_ID = "oimks47i3t5pnlb2vm7hy3rdfkwh6j";
const char* CLIENT_SECRET = "6jlc8bcesyj1cukiga7aji44cba78i";

class RaeptorApplication {
    private:
        GLFWwindow* window;
        Renderer* renderer;

    public:
        RaeptorApplication();
        ~RaeptorApplication();

        int update();

        ImGuiIO& getGui();
        GLFWwindow* getWindow();
};

GLFWwindow* create_window() {
    if (!glfwInit()) return nullptr;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RaeptorLab", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        glfwTerminate();
        return nullptr;
    }

    return window;
}

void create_gui(GLFWwindow* window) {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup ImGui style'
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

Texture testTexture;
Font font;
std::array<Sprite, 0> sprites;
int fontSize = 72;
RaeptorApplication::RaeptorApplication() {
    NFD_Init();
    this->window = create_window();
    create_gui(this->window);
    testTexture = Texture("assets/textures/texture.png");
    font = Font("assets/fonts/Alef-Bold.ttf", 72);
    for (size_t i = 0; i < sprites.size(); ++i) {
        sprites[i] = Sprite(testTexture);
        sprites[i].setPosition(glm::vec2(100.0f + (i%100) * 10, 100.0f + (i/100) * 10));
        sprites[i].setSize(glm::vec2(50.0f, 50.0f));
        sprites[i].setRotation(i*1.1f);
        sprites[i].setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprites[i].setAnchor(glm::vec2(0.5f, 0.5f));
        sprites[i].setVisibility(true);
    }
    this->renderer = new Renderer();
    for (auto& sprite : sprites) {
        sprite.addToRenderer(*this->renderer);
    }
    GLint maxSSBOSize;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
    std::cout << "Max SSBO size: " << maxSSBOSize / (1024.0 * 1024.0) << " MB\n";
}

RaeptorApplication::~RaeptorApplication() {
    // Destructor implementation
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(this->window);
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

    glfwGetWindowSize(window, &width, &height);
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
                glm::vec3 rectangleColor = sprite->getColor();

                ImGui::Indent();
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &rectanglePosition.x, 0.0f, width);
                ImGui::SliderFloat("Y", &rectanglePosition.y, 0.0f, height);
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
                glm::vec3 textColor = text->getColor();
                ImGui::Indent();
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &textPosition.x, 0.0f, width);
                ImGui::SliderFloat("Y", &textPosition.y, 0.0f, height);
                ImGui::SliderFloat2("Size", &textSize[0], 0.0f, 2000.0f);
                ImGui::SliderAngle("Rotation", &textRotation, 0.0f, 360.0f);
                ImGui::ColorEdit3("Color", &textColor[0]); // Example of color edit, replace with actual color
                ImGui::Unindent();
                text->setPosition(textPosition);
                text->setSize(textSize);
                text->setRotation(textRotation);
                text->setColor(textColor);

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
        sprite->setSize(glm::vec2(100.0f, 100.0f));
        sprite->setRotation(0.0f);
        sprite->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprite->setAnchor(glm::vec2(0.5f, 0.5f));
        sprite->setVisibility(true);
        nodes.push_back(Node{false, "sprite", sprite});
        sprite->addToRenderer(*this->renderer);
    }

    if (ImGui::Button("Add Text")) {
        Text* text = new Text(font, "Sample Text");
        text->setPosition(glm::vec2(100.0f, 100.0f));
        text->setSize(glm::vec2(100.0f, 100.0f));
        text->setRotation(0.0f);
        text->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        text->setVisibility(true);
        nodes.push_back(Node{false, "text", text});
        text->addToRenderer(*this->renderer);
    }

    ImGui::Separator();
    ImGui::End();

    ImGui::Render();

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->renderer->render(this->window);
    //this->renderer->clearBatches();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->window);

    if (glfwWindowShouldClose(this->window)) {
        this->renderer->clearBatches();
        for (auto& node : nodes) {
            delete node.sprite; // Clean up sprites
        }
        return -1;
    }
    return 0;
}

ImGuiIO& RaeptorApplication::getGui() {
    return ImGui::GetIO();
}

GLFWwindow* RaeptorApplication::getWindow() {
    return this->window;
}



int main() {
    RaeptorApplication app = RaeptorApplication();

    int8_t ret_value = 0;
    while (!(ret_value = app.update()));

    return ret_value;
}
