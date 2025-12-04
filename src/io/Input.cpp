#include <RaeptorCogs/IO/Input.hpp>
#include <GLFW/glfw3.h>

namespace RaeptorCogs {

namespace Singletons {

bool Input::isKeyPressed(Key key) {
    size_t index = static_cast<size_t>(key);
    if (index >= static_cast<size_t>(Key::KEY_COUNT)) {
        return false;
    }
    return keyStates.test(index);
};

bool Input::isKeyReleased(Key key) {
    size_t index = static_cast<size_t>(key);
    if (index >= static_cast<size_t>(Key::KEY_COUNT)) {
        return false;
    }
    return !keyStates.test(index);
};

void Input::update(Window &window) {
    prevKeyStates = keyStates;
    for (size_t i = static_cast<size_t>(Key::SPACE); i < static_cast<size_t>(Key::KEY_COUNT); ++i) {
        int state = glfwGetKey(window.getGLFWWindow(), static_cast<int>(i));
        if (state == GLFW_PRESS) {
            keyStates.set(i, true);
        } else {
            keyStates.set(i, false);
        }
    }
};

bool Mouse::isButtonPressed(int button) {
    if (button < 0 || button >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
        return false;
    }
    return buttonStates.test(static_cast<size_t>(button));
};

bool Mouse::isButtonReleased(int button) {
    if (button < 0 || button >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
        return false;
    }
    return !buttonStates.test(static_cast<size_t>(button));
};

double Mouse::getScrollX() const {
    return scroll.x;
};

double Mouse::getScrollY() const {
    return scroll.y;
};

glm::vec2 Mouse::getScroll() const {
    return scroll;
};

double Mouse::getX() const {
    return position.x;
};

double Mouse::getY() const {
    return position.y;
};

glm::vec2 Mouse::getPosition() const {
    return position;
};

uint64_t Mouse::getHoveredData() const {
    return hoveredData;
};

void Mouse::setHoveredData(uint64_t data) {
    hoveredData = data;
};

void Mouse::updateScroll(double xoffset, double yoffset) {
    scroll.x += static_cast<float>(xoffset);
    scroll.y += static_cast<float>(yoffset);
};

void Mouse::update(Window &window) {
    prevButtonStates = buttonStates;
    for (size_t i = 0; i < static_cast<size_t>(MouseButton::BUTTON_COUNT); ++i) {
        int state = glfwGetMouseButton(window.getGLFWWindow(), static_cast<int>(i));
        if (state == GLFW_PRESS) {
            buttonStates.set(i, true);
        } else {
            buttonStates.set(i, false);
        }
    }
    position = window.getMousePosition(); // Update mouse position
};

}

}