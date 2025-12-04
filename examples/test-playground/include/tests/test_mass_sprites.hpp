#pragma once
#include <RaeptorCogs/Sprite.hpp>

std::vector<RaeptorCogs::Sprite2D> &loadMassSprites(std::vector<std::string> &fileNames);
std::vector<RaeptorCogs::Sprite2D> &createMassSprites(RaeptorCogs::Texture &texture, size_t count, bool selectable);