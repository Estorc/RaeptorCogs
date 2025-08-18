#pragma once
#include <unordered_map>
#include <string>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <functional>
#include <sstream>


template<typename F, typename... Args>
auto call_factory(F&& f, Args&&... args)
{
    if constexpr (std::is_invocable_v<F, Args...>) {
        // lambda accepts arguments
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else if constexpr (std::is_invocable_v<F>) {
        // lambda accepts no arguments
        return std::invoke(std::forward<F>(f));
    } else {
        static_assert(sizeof(F) == 0, "Factory lambda has incompatible signature");
    }
}

template <typename... Args>
std::string getKey(const std::string& key, Args&&... args) {
    std::ostringstream oss;
    oss << key;
    ((oss << ":" << args), ...); // fold expression for all args
    return oss.str();
}
namespace RaeptorCogs::Singletons {
template<typename T>
class ResourceManager {
    private:
        std::unordered_map<std::string, T> resources;
    public:
        template <typename... Args>
        T *get(const std::string& key, Args&&... args) {
            auto it = resources.find(getKey(key, std::forward<Args>(args)...));
            if (it != resources.end()) {
                return &it->second;
            }
            return nullptr; // or throw an exception
        }
        template <typename... Args, typename F = std::function<T(Args&&...)>>
        T *create(F&& factory, const std::string& key, Args&&... args) {
            std::string compositeKey = getKey(key, std::forward<Args>(args)...);
            if constexpr (std::is_same_v<std::decay_t<F>, std::nullptr_t>) {
                // Default behavior → forward args to T constructor
                auto [insertedIt, success] = resources.emplace(
                    compositeKey,
                    T(key, std::forward<Args>(args)...)
                );
                return &insertedIt->second;
            } else {
                // Custom factory provided
                auto [insertedIt, success] = resources.emplace(
                    compositeKey,
                    call_factory(factory, key, std::forward<Args>(args)...)
                );
                return &insertedIt->second;
            }
        }
        template <typename... Args, typename F = std::function<T(Args&&...)>>
        T* get_or_create(F&& factory, const std::string& key, Args&&... args) {
            auto* resource = get(key, std::forward<Args>(args)...);
            if (!resource) {
                resource = create(std::forward<F>(factory), key, std::forward<Args>(args)...);
            }
            return resource;
        }
        
        void clear() {
            resources.clear(); // Clear all loaded resources
        }
};
}