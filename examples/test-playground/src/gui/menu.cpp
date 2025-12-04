#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Text.hpp>
#include <RaeptorCogs/Sprite.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <global.hpp>

#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

std::vector<Node> nodes;
int renderListID = 0;
int triangleCount = 0;

void createMenu(glm::vec2 windowSize) {
    RaeptorCogs::Renderer().CreateImGuiFrame();

    // Your ImGui code here
    float fps = 1.0f / ImGui::GetIO().DeltaTime;
    ImGui::Begin("Hello, ImGui!");
    ImGui::TextUnformatted(("This is a sample window ( " + std::to_string(fps) + "FPS )").c_str());

    ImGui::SliderInt("RenderListID", &renderListID, 0, 10);
    RaeptorCogs::Renderer().setRenderListID(renderListID);

    ImGui::SliderInt("Triangle Count", &triangleCount, 0, 50);

    for (size_t i = 0; i < nodes.size(); ++i) {
        Node& node = nodes[i];
        ImGui::PushID(static_cast<int>(i));  // Ensure unique ID per widget set

        if (ImGui::Button("x")) {
            node.sprite.reset(); // Clean up the sprite
            nodes.erase(nodes.begin() + static_cast<int>(i));
            ImGui::PopID();
            break; // break to avoid invalidating indices after erase
        }
        ImGui::SameLine();
        std::string label;
        if (node.type == "sprite") {
            label = "Sprite2D " + std::to_string(i);
        } else if (node.type == "text") {
            label = "Text " + std::to_string(i);
        } else {
            label = "Unknown Type " + std::to_string(i);
        }
        node.expanded = ImGui::CollapsingHeader(label.c_str());

        if (node.expanded) {
            if (node.type == "sprite") {
                ImGui::Text("Sprite2D Properties");

                RaeptorCogs::Sprite2D *sprite = dynamic_cast<RaeptorCogs::Sprite2D*>(node.sprite.get());
                glm::vec2 rectanglePosition = sprite->getPosition();
                glm::vec2 rectangleSize = sprite->getSize();
                float rectangleRotation = sprite->getRotation();
                float rectangleZIndex = sprite->getZIndex();
                glm::vec3 rectangleColor = sprite->getColor();
                bool visible = sprite->isVisible();

                ImGui::Indent();
                if (ImGui::Checkbox("Visible", &visible)) sprite->setVisibility(visible);
                ImGui::Text("Position");
                if (ImGui::SliderFloat("X", &rectanglePosition.x, 0.0f, windowSize.x)) sprite->setPosition(rectanglePosition);
                if (ImGui::SliderFloat("Y", &rectanglePosition.y, 0.0f, windowSize.y)) sprite->setPosition(rectanglePosition);
                if (ImGui::SliderFloat("Z Index", &rectangleZIndex, -100.0f, 100.0f)) sprite->setZIndex(rectangleZIndex);
                if (ImGui::SliderFloat2("Size", &rectangleSize[0], 0.0f, 2000.0f)) sprite->setSize(rectangleSize);
                if (ImGui::SliderAngle("Rotation", &rectangleRotation, 0.0f, 360.0f)) sprite->setRotation(rectangleRotation);
                if (ImGui::ColorEdit3("Color", &rectangleColor[0])) sprite->setColor(rectangleColor);
                
                if (ImGui::Button("Open Texture")) {
                    RaeptorCogs::OpenFileDialog([sprite](const RaeptorCogs::FileData& data, const std::string& name) {
                        RaeptorCogs::Texture &texture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(RaeptorCogs::UniqueKey(name), data, RaeptorCogs::TextureOptions{.priority = 150});
                        texture.onLoad = [sprite, &texture]() mutable {
                            std::cout << "Setting texture: " << texture->getID() << std::endl;
                            sprite->setTexture(texture);
                        };
                    }, RaeptorCogs::FileDialogFilters{ { "Image Files", "png,jpg,jpeg" } });
                }

                ImGui::Unindent();

            } else if (node.type == "text") {
                ImGui::Text("Text2D Properties");

                RaeptorCogs::Text2D *text = dynamic_cast<RaeptorCogs::Text2D*>(node.sprite.get());

                char textInput[256];
                strncpy_s(textInput, sizeof(textInput), text->getContent().c_str(), _TRUNCATE);
                if (ImGui::InputTextMultiline("Text2D Input", textInput, sizeof(textInput))) text->setContent(textInput);

                glm::vec2 textPosition = text->getPosition();
                glm::vec2 textRect = text->getSize();
                float textRotation = text->getRotation();
                float zIndex = text->getZIndex();
                glm::vec3 textColor = text->getColor();
                bool visible = text->isVisible();
                int wordWrapType = static_cast<int>(text->getWordWrapType());
                int alignment = static_cast<int>(text->getAlignment());
                float wordWrapWidth = text->getWordWrapWidth();
                float textSize = text->getTextSize();
                ImGui::Indent();
                if (ImGui::Checkbox("Visible", &visible)) text->setVisibility(visible);
                ImGui::Text("Position");
                if (ImGui::SliderFloat("X", &textPosition.x, 0.0f, windowSize.x)) text->setPosition(textPosition);
                if (ImGui::SliderFloat("Y", &textPosition.y, 0.0f, windowSize.y)) text->setPosition(textPosition);
                if (ImGui::SliderFloat("Z Index", &zIndex, -100.0f, 100.0f)) text->setZIndex(zIndex);
                if (ImGui::SliderFloat2("Size", &textRect[0], 0.0f, 2000.0f)) text->setSize(textRect);
                if (ImGui::SliderAngle("Rotation", &textRotation, 0.0f, 360.0f)) text->setRotation(textRotation);
                if (ImGui::ColorEdit3("Color", &textColor[0])) text->setColor(textColor);
                ImGui::Text("Font Size");
                if (ImGui::SliderFloat("##FontSize", &textSize, 8, 2000)) text->setTextSize(textSize);
                ImGui::Text("Word Wrap");
                if (ImGui::RadioButton("None", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::NONE))) text->setWordWrap(RaeptorCogs::TextWordWrap::NONE);
                ImGui::SameLine();
                if (ImGui::RadioButton("Word", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::WORD))) text->setWordWrap(RaeptorCogs::TextWordWrap::WORD);
                ImGui::SameLine();
                if (ImGui::RadioButton("Character", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::CHARACTER))) text->setWordWrap(RaeptorCogs::TextWordWrap::CHARACTER);
                if (wordWrapType != static_cast<int>(RaeptorCogs::TextWordWrap::NONE)) {
                    if (ImGui::SliderFloat("Width", &wordWrapWidth, 0.0f, 2000.0f)) text->setWordWrap(static_cast<RaeptorCogs::TextWordWrap>(wordWrapType), wordWrapWidth);
                }
                ImGui::Text("Alignment");
                if (ImGui::RadioButton("Left", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::LEFT))) text->setAlignment(RaeptorCogs::TextAlignment::LEFT);
                ImGui::SameLine();
                if (ImGui::RadioButton("Center", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::CENTER))) text->setAlignment(RaeptorCogs::TextAlignment::CENTER);
                ImGui::SameLine();
                if (ImGui::RadioButton("Right", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::RIGHT))) text->setAlignment(RaeptorCogs::TextAlignment::RIGHT);
                ImGui::Unindent();

                if (ImGui::Button("Open Font")) {
                    RaeptorCogs::OpenFileDialog([text](const RaeptorCogs::FileData& data, const std::string& name) {
                        RaeptorCogs::Font &font = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get_or_create(RaeptorCogs::UniqueKey(name), data, RaeptorCogs::FontOptions{.priority = 150});
                        font.onLoad = [text, &font]() mutable {
                            text->setFont(font);
                        };
                    }, RaeptorCogs::FileDialogFilters{ { "Font Files", "ttf" } });
                }
                ImGui::SameLine();
                ImGui::Text("Font Size");
                ImGui::SliderInt("##FontSize", &fontSize, 8, 128);
                
            }
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Sprite2D")) {
        RaeptorCogs::Sprite2D* sprite = new RaeptorCogs::Sprite2D(RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create("assets/textures/raeptor-cogs-logo.png"));
        sprite->setPosition(glm::vec2(100.0f, 100.0f));
        sprite->setSize(glm::vec2(256.0f, 256.0f));
        sprite->setRotation(0.0f);
        sprite->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprite->setAnchor(glm::vec2(0.5f, 0.5f));
        sprite->setVisibility(true);
        if (triangleCount > 2)
            sprite->setShape<RaeptorCogs::RegularPolygon>(triangleCount);
        else
            sprite->setShape<RaeptorCogs::Quad>();
        nodes.push_back(Node{false, "sprite", std::unique_ptr<RaeptorCogs::Sprite2D>(sprite)});
        RaeptorCogs::Renderer().add(*sprite);
    }

    if (ImGui::Button("Add Text2D")) {
        RaeptorCogs::Font &font = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get("assets/fonts/Alef-Bold.ttf");
        font.onLoad = [&font]() {
            RaeptorCogs::Text2D* text = new RaeptorCogs::Text2D(font, "Sample Text");
            text->setPosition(glm::vec2(100.0f, 100.0f));
            text->setSize(text->measureTextSize());
            text->setRotation(0.0f);
            text->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
            text->setAnchor(glm::vec2(0.5f, 0.5f));
            text->setVisibility(true);
            nodes.push_back(Node{false, "text", std::unique_ptr<RaeptorCogs::Text2D>(text)});
            RaeptorCogs::Renderer().add(*text);
        };
    }

    ImGui::Separator();
    ImGui::End();
}