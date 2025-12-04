/** ********************************************************************************
 * @section RaeptorCogs_Overview Overview
 * @file RaeptorCogs.hpp
 * @brief Main RaeptorCogs library header.
 * @details
 * Typical use cases:
 * - Including all RaeptorCogs functionalities in a single header
 * - Accessing core engine features such as rendering, resource management, input handling, and timing
 * *********************************************************************************
 * @section RaeptorCogs_Header Header
 * <RaeptorCogs/RaeptorCogs.hpp>
 ***********************************************************************************
 * @section RaeptorCogs_Metadata Metadata
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
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Memory.hpp>
#include <RaeptorCogs/IO/Texture.hpp>
#include <RaeptorCogs/Worker.hpp>
#include <RaeptorCogs/IO/Input.hpp>
#include <RaeptorCogs/Time.hpp>
#include <RaeptorCogs/Random.hpp>
#include <RaeptorCogs/Platform.hpp>
#include <RaeptorCogs/Singleton.hpp>
#include <functional>

/**
 * @namespace RaeptorCogs
 * @brief Main namespace for the RaeptorCogs library.
 * 
 * Contains everything related to the RaeptorCogs engine, including
 * rendering, resource management, input handling, timing, and more.
 * 
 * @code{.cpp}
 * #include <RaeptorCogs/RaeptorCogs.hpp>
 * // Use RaeptorCogs functionalities here
 * @endcode
 */
namespace RaeptorCogs{};

/**
 * @namespace std
 * @brief Standard C++ library namespace.
 * 
 * This namespace contains all the standard C++ library features.
 * 
 * @note This declaration is necessary for certain template specializations
 *       used within the RaeptorCogs library.
 */
namespace std{};

namespace RaeptorCogs {

    // ============================================================================
    //                               API FUNCTIONS
    // ============================================================================

    /**
     * @brief Initialize the RaeptorCogs engine.
     * 
     * Sets up necessary subsystems and prepares the engine for use.
     * 
     * @code{.cpp}
     * RaeptorCogs::Initialize();
     * @endcode
     */
    void Initialize();

    /**
     * @brief Main application loop.
     * 
     * Continuously calls the provided update function, processes input, and renders frames until the application exits.
     * 
     * @param updateFunction Function to be called each frame for updates.
     * @param window Pointer to the main application window.
     */
    void MainLoop(std::function<void(Window&)> updateFunction, Window &window);

    /**
     * @brief Start the main application loop.
     * 
     * @param updateFunction Function to be called each frame for updates.
     * @param window Reference to the main application window.
     * 
     * @code{.cpp}
     * RaeptorCogs::StartLoop([](RaeptorCogs::Window* mainWindow) {
     *     // Update logic here
     * }, mainWindow);
     * @endcode
     */
    void StartLoop(std::function<void(Window&)> updateFunction, Window &window);

    /**
     * @brief Destroy the RaeptorCogs engine and clean up resources.
     * 
     * @code{.cpp}
     * RaeptorCogs::Destroy();
     * @endcode
     */
    void Destroy();

    /**
     * @brief Check if the application is exiting.
     * 
     * @return true if the application is in the process of exiting, false otherwise.
     * 
     * @code{.cpp}
     * if (RaeptorCogs::IsExiting()) {
     *     // Handle exiting logic
     * }
     * @endcode
     */
    bool IsExiting();

    /**
     * @brief Get the current screen width.
     * 
     * @return Current screen width in pixels.
     * 
     * @code{.cpp}
     * double width = RaeptorCogs::GetScreenWidth();
     * @endcode
     */
    double GetScreenWidth();
    
    /**
     * @brief Get the current screen height.
     * 
     * @return Current screen height in pixels.
     * 
     * @code{.cpp}
     * double height = RaeptorCogs::GetScreenHeight();
     * @endcode
     */
    double GetScreenHeight();

    // ============================================================================
    //                               SINGLETON ACCESSORS
    // ============================================================================


    /**
     * @brief Access the global Renderer singleton.
     * 
     * @return Reference to the Renderer singleton.
     * 
     * @code{.cpp}
     * auto &renderer = RaeptorCogs::Renderer();
     * renderer.render(window);
     * @endcode
     */
    Singletons::Renderer& Renderer();

    /**
     * @brief Access the global TextureAtlasManager singleton.
     * 
     * @return Reference to the TextureAtlasManager singleton.
     * 
     * @code{.cpp}
     * auto &atlasManager = RaeptorCogs::TextureAtlasManager();
     * auto atlas = atlasManager.getAtlas(someKey);
     * @endcode
     */
    Singletons::TextureAtlasManager& TextureAtlasManager();

    /**
     * @brief Access the global Input singleton.
     * 
     * @return Reference to the Input singleton.
     * 
     * @code{.cpp}
     * if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::A)) {
     *     // 'A' key is currently pressed
     * }
     * @endcode
     */
    Singletons::Input& Input();

    /**
     * @brief Access the global Mouse singleton.
     * 
     * @return Reference to the Mouse singleton.
     * 
     * @code{.cpp}
     * if (RaeptorCogs::Mouse().isButtonPressed(RaeptorCogs::MouseButton::LEFT)) {
     *     // Left mouse button is currently pressed
     * }
     * @endcode
     */
    Singletons::Mouse& Mouse();

    /**
     * @brief Access the global Time singleton.
     * 
     * @return Reference to the Time singleton.
     * 
     * @code{.cpp}
     * double deltaTime = RaeptorCogs::Time().getDeltaTime();
     * @endcode
     */
    Singletons::Time& Time();
    
    /**
     * @brief Access the global Random singleton.
     * 
     * @return Reference to the Random singleton.
     * 
     * @code{.cpp}
     * int randInt = RaeptorCogs::Random().getInt(1, 10);
     * @endcode
     */
    Singletons::Random& Random();

    /**
     * @brief Access the global Platform singleton.
     * 
     * @return Reference to the Platform singleton.
     * 
     * @code{.cpp}
     * RaeptorCogs::Window* window = RaeptorCogs::Platform().createWindow(800, 600, "My Window");
     * @endcode
     */
    Singletons::Platform& Platform();

    /**
     * @brief Access the global ResourceManager singleton for a specific resource type.
     * 
     * @tparam T The type of resource managed by the ResourceManager.
     * @return Reference to the ResourceManager singleton for the specified resource type.
     * 
     * @code{.cpp}
     * auto &textureManager = RaeptorCogs::ResourceManager<RaeptorCogs::Texture>();
     * Texture &tex = textureManager.get_or_create("path/to/texture.png");
     * @endcode
     */
    template<typename T>
    Singletons::ResourceManager<T>& ResourceManager() {
        return RaeptorCogs::SingletonAccessor<Singletons::ResourceManager<T>>::get();
    }

    // ============================================================================
    //                               WORKER ACCESSORS
    // ============================================================================

    /**
     * @brief Access the global MainWorker singleton.
     * 
     * @return Reference to the MainWorker singleton (Main thread).
     * 
     * @code{.cpp}
     * RaeptorCogs::MainWorker().addJob([](){
     *     // Job logic here
     * });
     * @endcode
     */
    Singletons::MainWorker& MainWorker();

    /**
     * @brief Access the global ResourceWorker singleton.
     * 
     * @return Reference to the ResourceWorker singleton (Parallel thread).
     * 
     * @code{.cpp}
     * RaeptorCogs::ResourceWorker().addJob([](){
     *     // Job logic here
     * });
     * @endcode
     */
    Worker& ResourceWorker();
}