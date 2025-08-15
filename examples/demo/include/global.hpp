#pragma once
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <string>
#include <memory>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;


extern int fontSize;
extern RaeptorCogs::Texture testTexture;
extern RaeptorCogs::Font testFont;

struct Node {
    bool expanded = false; // Tracks whether the node is expanded
    std::string type;
    std::unique_ptr<RaeptorCogs::Graphic> sprite;    // Example setting
};