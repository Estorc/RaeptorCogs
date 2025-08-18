#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Text.hpp>
#include <RaeptorCogs/Sprite.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <global.hpp>

std::vector<Node> nodes;

void createMenu(glm::vec2 windowSize) {
    RaeptorCogs::Renderer().NewImGuiFrame();

    // Your ImGui code here
    float fps = static_cast<int>(1.0f / ImGui::GetIO().DeltaTime);
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text(("This is a sample window ( " + std::to_string(fps) + "FPS )").c_str());

    for (size_t i = 0; i < nodes.size(); ++i) {
        Node& node = nodes[i];
        ImGui::PushID(i);  // Ensure unique ID per widget set

        if (ImGui::Button("x")) {
            node.sprite.reset(); // Clean up the sprite
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

                RaeptorCogs::Sprite *sprite = dynamic_cast<RaeptorCogs::Sprite*>(node.sprite.get());
                glm::vec2 rectanglePosition = sprite->getPosition();
                glm::vec2 rectangleSize = sprite->getSize();
                float rectangleRotation = sprite->getRotation();
                float rectangleZIndex = sprite->getZIndex();
                glm::vec3 rectangleColor = sprite->getColor();
                bool visible = sprite->isVisible();

                ImGui::Indent();
                ImGui::Checkbox("Visible", &visible);
                ImGui::Text("Position");
                ImGui::SliderFloat("X", &rectanglePosition.x, 0.0f, windowSize.x);
                ImGui::SliderFloat("Y", &rectanglePosition.y, 0.0f, windowSize.y);
                ImGui::SliderFloat("Z Index", &rectangleZIndex, -100.0f, 100.0f);
                ImGui::SliderFloat2("Size", &rectangleSize[0], 0.0f, 2000.0f);
                ImGui::SliderAngle("Rotation", &rectangleRotation, 0.0f, 360.0f);
                ImGui::ColorEdit3("Color", &rectangleColor[0]); // Example of color edit, replace with actual color
                
                if (ImGui::Button("Open Texture")) {
                    RaeptorCogs::OpenFileDialog([sprite](const RaeptorCogs::FileData& data, const std::string& name) {
                        RaeptorCogs::Texture *texture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(name, [&] {
                            return RaeptorCogs::Texture(RaeptorCogs::LoadImageFromMemory(data));
                        });
                        sprite->setTexture(*texture);
                    }, RaeptorCogs::FileDialogFilters{ { "Image Files", "png,jpg,jpeg" } });
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

                RaeptorCogs::Text *text = dynamic_cast<RaeptorCogs::Text*>(node.sprite.get());

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
                ImGui::SliderFloat("X", &textPosition.x, 0.0f, windowSize.x);
                ImGui::SliderFloat("Y", &textPosition.y, 0.0f, windowSize.y);
                ImGui::SliderFloat("Z Index", &zIndex, -100.0f, 100.0f);
                ImGui::SliderFloat2("Size", &textSize[0], 0.0f, 2000.0f);
                ImGui::SliderAngle("Rotation", &textRotation, 0.0f, 360.0f);
                ImGui::ColorEdit3("Color", &textColor[0]); // Example of color edit, replace with actual color
                ImGui::Text("Word Wrap");
                ImGui::RadioButton("None", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::NONE));
                ImGui::SameLine();
                ImGui::RadioButton("Word", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::WORD));
                ImGui::SameLine();
                ImGui::RadioButton("Character", &wordWrapType, static_cast<int>(RaeptorCogs::TextWordWrap::CHARACTER));
                if (wordWrapType != static_cast<int>(RaeptorCogs::TextWordWrap::NONE)) {
                    ImGui::SliderFloat("Width", &wordWrapWidth, 0.0f, 2000.0f);
                    text->setWordWrap(static_cast<RaeptorCogs::TextWordWrap>(wordWrapType), wordWrapWidth);
                } else {
                    text->setWordWrap(RaeptorCogs::TextWordWrap::NONE);
                }
                ImGui::Text("Alignment");
                ImGui::RadioButton("Left", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::LEFT));
                ImGui::SameLine();
                ImGui::RadioButton("Center", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::CENTER));
                ImGui::SameLine();
                ImGui::RadioButton("Right", &alignment, static_cast<int>(RaeptorCogs::TextAlignment::RIGHT));
                ImGui::Unindent();
                text->setAlignment(static_cast<RaeptorCogs::TextAlignment>(alignment));
                text->setPosition(textPosition);
                text->setSize(textSize);
                text->setRotation(textRotation);
                text->setColor(textColor);
                text->setZIndex(zIndex);
                text->setVisibility(visible);

                if (ImGui::Button("Open Font")) {
                    RaeptorCogs::OpenFileDialog([text](const RaeptorCogs::FileData& data, const std::string& name) {
                        RaeptorCogs::Font *font = RaeptorCogs::ResourceManager<RaeptorCogs::Font>().get_or_create(name, [&] {
                            return RaeptorCogs::Font(data, fontSize);
                        });
                        text->setFont(*font);
                    }, RaeptorCogs::FileDialogFilters{ { "Font Files", "ttf" } });
                }
                ImGui::SameLine();
                ImGui::Text("Font Size");
                ImGui::SliderInt("##FontSize", &fontSize, 8, 128);
                
            }
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Sprite")) {
        RaeptorCogs::Sprite* sprite = new RaeptorCogs::Sprite(testTexture);
        sprite->setPosition(glm::vec2(100.0f, 100.0f));
        sprite->setSize(glm::vec2(256.0f, 256.0f));
        sprite->setRotation(0.0f);
        sprite->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        sprite->setAnchor(glm::vec2(0.5f, 0.5f));
        sprite->setVisibility(true);
        nodes.push_back(Node{false, "sprite", std::unique_ptr<RaeptorCogs::Sprite>(sprite)});
        sprite->addToRenderer(RaeptorCogs::Renderer());
    }

    if (ImGui::Button("Add Text")) {
        RaeptorCogs::Text* text = new RaeptorCogs::Text(testFont, "Sample Text");
        text->setPosition(glm::vec2(100.0f, 100.0f));
        text->setSize(text->measureTextSize());
        text->setRotation(0.0f);
        text->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        text->setAnchor(glm::vec2(0.5f, 0.5f));
        text->setVisibility(true);
        nodes.push_back(Node{false, "text", std::unique_ptr<RaeptorCogs::Text>(text)});
        text->addToRenderer(RaeptorCogs::Renderer());
    }

    ImGui::Separator();
    ImGui::End();
}