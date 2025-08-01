#include <RaeptorLab/memory.hpp>

Texture& RessourceManager::loadTextureFromFile(const std::string& filepath) {
    if (textures.find(filepath) == textures.end()) {
        Texture texture(filepath.c_str());
        textures[filepath] = texture; // Store the texture in the map
    }
    return textures[filepath]; // Return the existing texture
}

Font& RessourceManager::loadFontFromFile(const std::string& filepath, int fontSize) {
    if (fonts.find(filepath) == fonts.end() || fonts[filepath].find(fontSize) == fonts[filepath].end()) {
        fonts[filepath][fontSize] = Font(filepath.c_str(), fontSize); // Store the font in the map
    }
    return fonts[filepath][fontSize]; // Return the existing font
}

void RessourceManager::clear() {
    textures.clear(); // Clear all loaded textures
    for (auto& fontPair : fonts) {
        fontPair.second.clear(); // Clear all loaded fonts for each file
    }
}

RessourceManager ressourceManager = RessourceManager(); // Global instance of RessourceManager