/** ********************************************************************************
 * @section IO_Input_Overview Overview
 * @file Input.hpp
 * @brief High-level input handling and management.
 * @details
 * Typical use cases:
 * - Querying the state of keyboard keys
 * - Querying mouse position and button states
 * *********************************************************************************
 * @section IO_Input_Header Header
 * <RaeptorCogs/IO/Input.hpp>
 ***********************************************************************************
 * @section IO_Input_Metadata Metadata
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
#include <RaeptorCogs/Singleton.hpp>
#include <RaeptorCogs/Window.hpp>
#include <glm/vec2.hpp>
#include <bitset>


namespace RaeptorCogs {

/**
 * @brief Key enumeration for keyboard input.
 * 
 * Represents various keys on the keyboard, mapped to GLFW key codes.
 */
enum class Key : uint32_t {
    UNKNOWN = 0,
    SPACE = 32,
    APOSTROPHE = 39, /* ' */
    PLUS = 334, /* + */
    MINUS = 333, /* - */
    COMMA = 44, /* , */
    PERIOD = 46, /* . */
    SLASH = 47, /* / */
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    SEMICOLON = 59, /* ; */
    EQUAL = 61, /* = */
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LEFT_BRACKET = 91, /* [ */
    BACKSLASH = 92, /* \ */
    RIGHT_BRACKET = 93, /* ] */
    GRAVE_ACCENT = 96, /* ` */
    WORLD_1 = 161, /* non-US #1 */
    WORLD_2 = 162, /* non-US #2 */
    
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE_KEY = 261,

    RIGHT_ARROW = 262,
    LEFT_ARROW = 263,
    DOWN_ARROW = 264,
    UP_ARROW = 265,

    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,

    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348,
    KEY_COUNT = 348
};


namespace Singletons {

/**
 * @brief Input handling class.
 * 
 * Provides methods to query the state of keyboard keys.
 * @code{.cpp}
 * if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::SPACE)) {
 *     // Space key is currently pressed
 * }
 * @endcode
 */
class Input {
    private:
    
        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Key state bitsets.
         * 
         * Stores the current states of all keys.
         */
        std::bitset<static_cast<size_t>(Key::KEY_COUNT)> keyStates; // GLFW_KEY_LAST is 348

        /**
         * @brief Previous key state bitsets.
         * 
         * Stores the previous states of all keys for edge detection.
         */
        std::bitset<static_cast<size_t>(Key::KEY_COUNT)> prevKeyStates; // GLFW_KEY_LAST is 348

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Private constructor.
         */
        Input() = default;

        /**
         * @brief Private destructor.
         */
        ~Input() = default;

        /**
         * @brief Friend declaration for SingletonAccessor.
         * 
         * Necessary for Singleton pattern implementation.
         */
        friend SingletonAccessor<Input>;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Check if a key is currently pressed.
         * 
         * @param key Key to check.
         * @return true if the key is pressed, false otherwise.
         * 
         * @code{.cpp}
         * if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::A)) {
         *     // 'A' key is currently pressed
         * }
         * @endcode
         */
        bool isKeyPressed(Key key);

        /**
         * @brief Check if a key was released.
         * 
         * @param key Key to check.
         * @return true if the key was released, false otherwise.
         * 
         * @code{.cpp}
         * if (RaeptorCogs::Input().isKeyReleased(RaeptorCogs::Key::ESCAPE)) {
         *     // Escape key was just released
         * }
         * @endcode
         */
        bool isKeyReleased(Key key);

        /**
         * @brief Update the input states.
         * 
         * Is called once per frame to update key states.
         * 
         * @param window Reference to the Window instance.
         * 
         * @note Updates are already handled internally; manual calls are not required.
         */
        void update(Window &window);
};

/**
 * @brief Mouse button enumeration.
 * 
 * Represents various mouse buttons, mapped to GLFW button codes.
 */
enum class MouseButton : uint8_t {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    BUTTON_7 = 6,
    BUTTON_8 = 7,
    BUTTON_COUNT = 8
};

/**
 * @brief Mouse input handling class.
 * 
 * Provides methods to query the state of mouse buttons and cursor position.
 * @code{.cpp}
 * if (RaeptorCogs::Mouse().isButtonPressed(RaeptorCogs::MouseButton::LEFT)) {
 *     // Left mouse button is currently pressed
 * }
 * double mouseX = RaeptorCogs::Mouse().getX();
 * @endcode
 */
class Mouse {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Mouse button state bitsets.
         * 
         * Stores the current states of all mouse buttons.
         */
        std::bitset<static_cast<size_t>(MouseButton::BUTTON_COUNT)> buttonStates; // GLFW_MOUSE_BUTTON_LAST is 7

        /**
         * @brief Previous mouse button state bitsets.
         * 
         * Stores the previous states of all mouse buttons for edge detection.
         */
        std::bitset<static_cast<size_t>(MouseButton::BUTTON_COUNT)> prevButtonStates; // GLFW_MOUSE_BUTTON_LAST is 7

        /**
         * @brief Scroll offset.
         * 
         * Stores the accumulated scroll offsets.
         */
        glm::vec2 scroll = glm::vec2(0.0f);

        /**
         * @brief Current mouse position.
         * 
         * Stores the current cursor position.
         */
        glm::vec2 position = glm::vec2(0.0f);

        /**
         * @brief Hovered data identifier.
         * 
         * Stores an identifier for the currently hovered object.
         * 
         * @note Can be used for object picking or UI interactions.
         */
        uint64_t hoveredData = 0;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Private constructor.
         */
        Mouse() = default;

        /**
         * @brief Private destructor.
         */
        ~Mouse() = default;

        /**
         * @brief Friend declaration for SingletonAccessor.
         * 
         * Necessary for Singleton pattern implementation.
         */
        friend SingletonAccessor<Mouse>;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Check if a mouse button is currently pressed.
         * 
         * @param button Mouse button to check.
         * @return true if the button is pressed, false otherwise.
         * 
         * @code{.cpp}
         * if (RaeptorCogs::Mouse().isButtonPressed(RaeptorCogs::MouseButton::LEFT)) {
         *     // Left mouse button is currently pressed
         * }
         * @endcode
         */
        bool isButtonPressed(int button); // button: 0-7

        /**
         * @brief Check if a mouse button was released.
         * 
         * @param button Mouse button to check.
         * @return true if the button was released, false otherwise.
         * 
         * @code{.cpp}
         * if (RaeptorCogs::Mouse().isButtonReleased(RaeptorCogs::MouseButton::RIGHT)) {
         *     // Right mouse button was just released
         * }
         * @endcode
         */
        bool isButtonReleased(int button); // button: 0-7

        /**
         * @brief Get the horizontal scroll offset.
         * 
         * @return Horizontal scroll offset.
         */
        double getScrollX() const;

        /**
         * @brief Get the vertical scroll offset.
         * 
         * @return Vertical scroll offset.
         */
        double getScrollY() const;

        /**
         * @brief Get the scroll offset as a vector.
         * 
         * @return Scroll offset (x, y) as a glm::vec2.
         */
        glm::vec2 getScroll() const;

        /**
         * @brief Get the current X position of the mouse cursor.
         * 
         * @return X position of the cursor.
         */
        double getX() const;

        /**
         * @brief Get the current Y position of the mouse cursor.
         * 
         * @return Y position of the cursor.
         */
        double getY() const;

        /**
         * @brief Get the current position of the mouse cursor.
         * 
         * @return Position of the cursor as a glm::vec2.
         */
        glm::vec2 getPosition() const;

        /**
         * @brief Get the hovered data identifier.
         * 
         * @return Hovered data identifier.
         */
        uint64_t getHoveredData() const;

        /**
         * @brief Set the hovered data identifier.
         * 
         * @param data Hovered data identifier to set.
         * 
         * @note This is done internally by the system.
         */
        void setHoveredData(uint64_t data);

        /**
         * @brief Update the scroll offsets.
         * 
         * @param xoffset Horizontal scroll offset.
         * @param yoffset Vertical scroll offset.
         * 
         * @note This is done internally by the system.
         */
        void updateScroll(double xoffset, double yoffset);

        /**
         * @brief Update the mouse.
         * 
         * Is called once per frame to update button states and cursor position.
         * 
         * @param window Reference to the Window instance.
         * 
         * @note Updates are already handled internally; manual calls are not required.
         */
        void update(Window &window);

};

}

}