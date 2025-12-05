#include <tests/test_mass_sprites.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <nlohmann/json.hpp>

#include <array>

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

std::vector<RaeptorCogs::Sprite2D> _sprites;



float x = 0.0f, y = 0.0f;
std::vector<RaeptorCogs::Sprite2D> &loadMassSprites(std::vector<std::string> &fileNames) {

    nlohmann::json jsonData;
    std::filesystem::path folderPath = std::string(reinterpret_cast<const char*>(RaeptorCogs::LoadFile("protected/folderPath.txt").data())); // change to your folder
    std::filesystem::path jsonFile = "tags.json"; // change to your path
    std::vector<std::string> files;

    // Load and parse the JSON file
    try {
        jsonData = nlohmann::json::parse(RaeptorCogs::LoadFile((folderPath / jsonFile).string().c_str()));
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
        //return;
    }

    _sprites.reserve(jsonData.size());
    RaeptorCogs::Texture &testTexture = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().create("assets/textures/raeptor-cogs-logo.png");
    for (const auto& [key, value] : jsonData.items()) {
        std::string filePath = (folderPath / value["file"]).string();
        fileNames.push_back(filePath);
        RaeptorCogs::Texture& tex = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>().get_or_create(filePath.c_str(), RaeptorCogs::TextureOptions{.s_width = 0, .s_height = 150});
        tex.onLoad = [&tex, &testTexture, key, size = jsonData.size()]() mutable {
            float aspectRatio = static_cast<float>(tex->getWidth()) / static_cast<float>(tex->getHeight());
            float spriteHeight = 150.0f; // Fixed height for all _sprites
            float spriteWidth = spriteHeight * aspectRatio;

            if (x + spriteWidth > 1920.0f) { // Assuming a window width of 1920
                x = 0.0f;
                y += spriteHeight + 10; // Move to next row
            }

            RaeptorCogs::Sprite2D *image = new RaeptorCogs::Sprite2D(tex);
            image->setPosition(glm::vec2(0, 0));
            image->setSize(glm::vec2(spriteWidth, spriteHeight));
            image->setAnchor(glm::vec2(0.5f, 0.5f));
            RaeptorCogs::Renderer().add(*image);


            _sprites.push_back(RaeptorCogs::Sprite2D(testTexture));
            _sprites.back().setPosition(glm::vec2(x + spriteWidth / 2, y + spriteHeight / 2));
            _sprites.back().setSize(glm::vec2(spriteWidth, spriteHeight));
            _sprites.back().setAnchor(glm::vec2(0.5f, 0.5f));
            _sprites.back().setWritingMaskID(static_cast<int>(_sprites.size())); // Unique ID for each sprite
            _sprites.back().setReadingMaskID(0);
            _sprites.back().setScale(glm::vec2(1.0f, 1.0f));
            _sprites.back().addChild(image);

            x += spriteWidth + 10; // 10 pixels gap
            //_sprites.back().setZIndex(static_cast<float>(_sprites.size()));
            RaeptorCogs::Renderer().add(_sprites.back());
            //std::cout << key << " / " << size << std::endl;
        };
    }
    return _sprites;
}

std::vector<RaeptorCogs::Sprite2D> &createMassSprites(RaeptorCogs::Texture &texture, size_t count, bool selectable) {
    texture.onLoad = [&texture, count, selectable]() mutable {
        _sprites.reserve(count);
        float x, y;
        x = y = 0.0f;
        for (size_t i = 0; i < count; i++) {

            if (selectable) {
                RaeptorCogs::Sprite2D *sprite = new RaeptorCogs::Sprite2D(texture);
                sprite->setPosition(glm::vec2(0, 0));
                sprite->setSize(glm::vec2(6.0f, 6.0f));
                sprite->setAnchor(glm::vec2(0.5f, 0.5f));
                RaeptorCogs::Renderer().add(*sprite);

                _sprites.push_back(RaeptorCogs::Sprite2D(texture));
                _sprites.back().setPosition(glm::vec2(x, y));
                _sprites.back().setSize(glm::vec2(6.0f, 6.0f));
                _sprites.back().setAnchor(glm::vec2(0.5f, 0.5f));
                _sprites.back().setWritingMaskID(static_cast<int>(i) + 1); // Unique ID for each sprite
                _sprites.back().setReadingMaskID(0);
                //_sprites.back().setRotation(i);
                _sprites.back().setVisibility(true);
                //_sprites.back().setZIndex(static_cast<float>(i));
                _sprites.back().addChild(sprite);
            } else {
                _sprites.push_back(RaeptorCogs::Sprite2D(texture));
                _sprites.back().setPosition(glm::vec2(x, y));
                _sprites.back().setSize(glm::vec2(6.0f, 6.0f));
                _sprites.back().setAnchor(glm::vec2(0.5f, 0.5f));
                _sprites.back().setVisibility(true);
                _sprites.back().setZIndex(static_cast<float>(i)/1000.0f);
            }

            x += 7.0f; // 7 pixels gap
            if (x + 7.0f > 1920.0f) { // Assuming a window width of 1920
                x = 0.0f;
                y += 7.0f; // Move to next row
            }
            RaeptorCogs::Renderer().add(_sprites.back());
            //_sprites.back().setZIndex(static_cast<float>(i)/100.0f);
        }
    };
    return _sprites;
}