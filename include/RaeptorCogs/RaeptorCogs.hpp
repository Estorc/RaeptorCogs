#pragma once
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <functional>


namespace RaeptorCogs {
    void Initialize();
    void StartLoop(std::function<void()> updateFunction, Window* window);
    void Destroy();

    Singletons::Renderer& Renderer();
    Singletons::TextureAtlasManager& TextureAtlasManager();
    template<typename T>
    Singletons::RessourceManager<T>& RessourceManager() {
        static Singletons::RessourceManager<T> instance;
        return instance;
    }

    double GetScreenWidth();
    double GetScreenHeight();
}