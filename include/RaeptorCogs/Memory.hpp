/** ********************************************************************************
 * @section Memory_Overview Overview
 * @file Memory.hpp
 * @brief High-level memory management utilities.
 * @details
 * Typical use cases:
 * - Managing memory allocation and deallocation.
 * - Providing resource management functionalities.
 * *********************************************************************************
 * @section Memory_Header Header
 * <RaeptorCogs/Memory.hpp>
 ***********************************************************************************
 * @section Memory_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/IO/Font.hpp>
#include <RaeptorCogs/Singleton.hpp>
#include <unordered_map>
#include <string>
#include <functional>
#include <sstream>

namespace RaeptorCogs {

/**
 * @brief Helper trait to check if a type is streamable to std::ostringstream.
 */
template<typename T, typename = void>
struct is_streamable : std::false_type {};

/**
 * @brief Specialization for streamable types.
 */
template<typename T>
struct is_streamable<T, std::void_t<
    decltype(std::declval<std::ostringstream&>() << std::declval<T>())
>> : std::true_type {};


/**
 * @brief Struct to create unique keys for resource management.
 * 
 * Used by ResourceManager to uniquely identify resources based on constructor arguments.
 * 
 * Typical use cases:
 * - Generating unique keys for textures, fonts, and other resources.
 * 
 * @see ResourceManager
 */
struct UniqueKey {
    /**
     * @brief The computed unique key value.
     * 
     * Holds the string representation of the unique key.
     */
    std::string value;

    /**
     * @brief Constructor to create a UniqueKey from various arguments.
     * 
     * @param args Arguments used to compute the unique key.
     * 
     * @note This constructor concatenates the string representations of the provided arguments to form a unique key.
     */
    template<typename... Args,
            typename = std::enable_if_t<(is_streamable<Args>::value && ...)>>
    UniqueKey(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        value = oss.str();
    }
    
    /**
     * @brief Default constructor for an empty UniqueKey.
     * 
     * Initializes the unique key value to an empty string.
     */
    UniqueKey() : value("") {}
};

}

namespace RaeptorCogs::Singletons {

/**
 * @brief ResourceManager class template.
 * 
 * Provides resource management functionalities for various resource types.
 * 
 * Typical use cases:
 * - Loading, retrieving, and managing resources like textures and fonts.
 * 
 * @tparam T Type of resource to manage (e.g., Texture, Font).
 * @code{.cpp}
 * // Example usage:
 * auto &textureManager = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>();
 * Texture &tex = textureManager.get_or_create("path/to/texture.png");
 * @endcode
 */
template<typename T>
class ResourceManager {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Map to store resources.
         * 
         * Maps unique keys to shared pointers of resources.
         */
        std::unordered_map<std::string, std::shared_ptr<T>> resources;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Default constructor for ResourceManager.
         * 
         * Initializes the ResourceManager instance.
         * 
         * @note This constructor is private to enforce singleton behavior.
         */
        ResourceManager() = default;
        friend SingletonAccessor<ResourceManager<T>>;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Check if a resource exists.
         * 
         * @param args Arguments used to compute the unique key.
         * @return True if the resource exists, false otherwise.
         * 
         * @note This method uses the UniqueKey struct to compute the key from the provided arguments.
         */
        template<typename... Args>
        bool exists(Args&&... args) const {
            return resources.find(UniqueKey(std::forward<Args>(args)...).value) != resources.end();
        }

        /**
         * @brief Get a resource by its unique key.
         * 
         * @param args Arguments used to compute the unique key.
         * @return Reference to the requested resource.
         * 
         * @warning Throws a runtime error if the resource is not found.
         */
        template <typename... Args>
        T &get(Args&&... args) {
            auto it = resources.find(UniqueKey(std::forward<Args>(args)...).value);
            if (it == resources.end()) {
                throw std::runtime_error("Resource not found");
            }
            return *(it->second);
        }

        /**
         * @brief Create a new resource.
         * 
         * @param key UniqueKey used to identify the resource.
         * @param args Arguments forwarded to the resource constructor.
         * @return Reference to the newly created resource.
         * 
         * @note This method uses the provided UniqueKey to store the resource.
         */
        template <typename... Args>
        T &create(UniqueKey key, Args&&... args) {
            std::string computedKey = key.value;
            // Default behavior → forward args to T constructor
            auto [insertedIt, success] = resources.emplace(
                computedKey,
                std::make_shared<T>(std::forward<Args>(args)...)
            );
            return *(insertedIt->second);
        }

        /**
         * @brief Create a new resource.
         * 
         * @param args Arguments forwarded to the resource constructor.
         * @return Reference to the newly created resource.
         * 
         * @note This method computes a UniqueKey from the provided arguments to store the resource.
         */
        template <typename... Args>
        T &create(Args&&... args) {
            std::string computedKey = UniqueKey(std::forward<Args>(args)...).value;
            // Default behavior → forward args to T constructor
            auto [insertedIt, success] = resources.emplace(
                computedKey,
                std::make_shared<T>(std::forward<Args>(args)...)
            );
            return *(insertedIt->second);
        }

        /**
         * @brief Get or create a resource.
         * 
         * @param key UniqueKey used to identify the resource.
         * @param args Arguments forwarded to the resource constructor if creation is needed.
         * @return Reference to the requested or newly created resource.
         * 
         * @note This method checks for the existence of the resource before creating it.
         */
        template <typename... Args>
        T &get_or_create(UniqueKey key, Args&&... args) {
            std::string &computedKey = key.value;

            if (this->exists(computedKey)) {
                return get(computedKey);
            } else {
                return create(key, std::forward<Args>(args)...);
            }
        }

        /**
         * @brief Get or create a resource.
         * 
         * @param args Arguments forwarded to the resource constructor if creation is needed.
         * @return Reference to the requested or newly created resource.
         * 
         * @note This method computes a UniqueKey from the provided arguments to check for existence before creating the resource.
         */
        template<typename... Args>
        T& get_or_create(Args&&... args) {
            // Compute a key from the arguments
            UniqueKey key(std::forward<Args>(args)...);
            const std::string& computedKey = key.value;

            if (this->exists(computedKey)) {
                return get(computedKey);
            } else {
                return create(key, std::forward<Args>(args)...);
            }
        }

        /**
         * @brief Remove a resource by its unique key.
         * 
         * @param key UniqueKey used to identify the resource.
         * 
         * @note This method removes the resource associated with the provided UniqueKey.
         * @warning This don't immediately free the memory if there are other shared pointers to the resource.
         */
        void remove(UniqueKey key) {
            std::string &computedKey = key.value;
            resources.erase(computedKey);
        }

        /**
         * @brief Remove a resource by its unique key.
         * 
         * @param args Arguments used to compute the unique key.
         * 
         * @note This method computes a UniqueKey from the provided arguments to identify the resource to remove.
         * @warning This don't immediately free the memory if there are other shared pointers to the resource
         */
        template<typename... Args>
        void remove(Args&&... args) {
            std::string key = UniqueKey(std::forward<Args>(args)...).value;
            resources.erase(key);
        }

        /**
         * @brief Clear all resources.
         * 
         * @note This method removes all resources managed by the ResourceManager.
         * @warning This don't immediately free the memory if there are other shared pointers to the resources.
         */
        void clear() {
            resources.clear(); // Clear all loaded resources
        }
};
};