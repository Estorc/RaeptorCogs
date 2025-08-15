#pragma once
#include <unordered_map>
#include <string>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/IO/Font.hpp>
namespace RaeptorCogs::Singletons {
template<typename T>
class RessourceManager {
    private:
        std::unordered_map<std::string, T> ressources;
    public:
        //Texture& loadTextureFromFile(const std::string& filepath);
        //Font& loadFontFromFile(const std::string& filepath, int fontSize = 16);
        T *get(const std::string& name) {
            auto it = ressources.find(name);
            if (it != ressources.end()) {
                return &it->second;
            }
            return nullptr; // or throw an exception
        }
        T *create(const std::string& name) {
            ressources[name] = T();
            return &ressources[name];
        }
        void clear() {
            ressources.clear(); // Clear all loaded resources
        }
};
}