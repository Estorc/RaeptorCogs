#pragma once
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/Graphic.hpp>
#include <string>
#include <memory>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;


extern int fontSize;

struct Node {
    bool expanded = false; // Tracks whether the node is expanded
    std::string type;
    std::unique_ptr<RaeptorCogs::Graphic2D> sprite;    // Example setting
};