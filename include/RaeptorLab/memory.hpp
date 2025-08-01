#pragma once
#include <unordered_map>
#include <string>
#include <RaeptorLab/texture.hpp>
#include <RaeptorLab/io/font.hpp>

class RessourceManager {
    private:
        std::unordered_map<std::string, Texture> textures; // Map of texture file paths to Texture objects
        std::unordered_map<std::string, std::unordered_map<int, Font>> fonts; // Map of font file paths to Font objects
    public:
        Texture& loadTextureFromFile(const std::string& filepath);
        Font& loadFontFromFile(const std::string& filepath, int fontSize = 16);
        void clear();
};
extern RessourceManager ressourceManager;