#pragma once
#include <unordered_map>
#include <string>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/IO/Font.hpp>
namespace RaeptorCogs::Singletons {
template<typename T>
class ResourceManager {
    private:
        std::unordered_map<std::string, T> resources;
    public:
        T *get(const std::string& name) {
            auto it = resources.find(name);
            if (it != resources.end()) {
                return &it->second;
            }
            return nullptr; // or throw an exception
        }
        template <typename F = std::nullptr_t>
        T *create(const std::string& name, F&& factory = nullptr) {
            if constexpr (std::is_same_v<std::decay_t<F>, std::nullptr_t>) {
                // No factory passed → default construction
                auto [insertedIt, success] = resources.emplace(name, T());
                return &insertedIt->second;
            } else {
                // Factory passed → call it to build the resource
                auto [insertedIt, success] = resources.emplace(name, std::forward<F>(factory)());
                if (success) {
                    return &insertedIt->second;
                } else {
                    return nullptr; // or throw an exception if needed
                }
            }
        }
        template <typename F = std::nullptr_t>
        T* get_or_create(const std::string& name, F&& factory = nullptr) {
            auto* resource = get(name);
            if (!resource) {
                resource = create(name, factory);
            }
            return resource;
        }
        void clear() {
            resources.clear(); // Clear all loaded resources
        }
};
}