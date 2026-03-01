#include <GLFW/glfw3.h>
#include <RaeptorCogs/IO/Input.hpp>


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
  for (
      size_t i = static_cast<size_t>(Key::SPACE); i < static_cast<size_t>(Key::KEY_COUNT);
      ++i) {
    int state = glfwGetKey(window.getGLFWWindow(), static_cast<int>(i));
    if (state == GLFW_PRESS) {
      keyStates.set(i, true);
    } else {
      keyStates.set(i, false);
    }
  }
};

bool Mouse::isButtonPressed(MouseButton button) {
  if (static_cast<int>(button) < 0 ||
      static_cast<int>(button) >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
    return false;
  }
  return buttonStates.test(static_cast<size_t>(button));
};

bool Mouse::isButtonReleased(MouseButton button) {
  if (static_cast<int>(button) < 0 ||
      static_cast<int>(button) >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
    return false;
  }
  return !buttonStates.test(static_cast<size_t>(button));
};

bool Mouse::isButtonJustPressed(MouseButton button) {
  if (static_cast<int>(button) < 0 ||
      static_cast<int>(button) >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
    return false;
  }
  size_t index = static_cast<size_t>(button);
  return buttonStates.test(index) && !prevButtonStates.test(index);
};

bool Mouse::isButtonJustReleased(MouseButton button) {
  if (static_cast<int>(button) < 0 ||
      static_cast<int>(button) >= static_cast<int>(MouseButton::BUTTON_COUNT)) {
    return false;
  }
  size_t index = static_cast<size_t>(button);
  return !buttonStates.test(index) && prevButtonStates.test(index);
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

glm::vec2 Mouse::getDeltaPosition() const {
  return position - prevPosition;
};

uint64_t Mouse::getHoveredData() const {
  return hoveredData;
};

void Mouse::setHoveredData(uint64_t data) {
  hoveredData = data;
};

void Mouse::updateScroll(double xoffset, double yoffset) {
  scroll.x = static_cast<float>(xoffset);
  scroll.y = static_cast<float>(yoffset);
};

void Mouse::update(Window &window) {
  scroll           = glm::vec2(0.0f);
  prevButtonStates = buttonStates;
  for (size_t i = 0; i < static_cast<size_t>(MouseButton::BUTTON_COUNT); ++i) {
    int state = glfwGetMouseButton(window.getGLFWWindow(), static_cast<int>(i));
    if (state == GLFW_PRESS) {
      buttonStates.set(i, true);
    } else {
      buttonStates.set(i, false);
    }
  }
  prevPosition = position;
  position     = window.getMousePosition(); // Update mouse position
};

} // namespace Singletons

} // namespace RaeptorCogs