#include <RaeptorLab/window.hpp>
#include <iostream>

Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor, GLFWwindow* share) 
    : title(title) {
    this->window = glfwCreateWindow(width, height, title.c_str(), monitor, share);
    if (!this->window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
    }
    glfwMakeContextCurrent(this->window);
}

Window::~Window() {
    if (this->window) {
        glfwDestroyWindow(this->window);
    }
}

GLFWwindow* Window::getGLFWWindow() const {
    return this->window;
}

int Window::getWidth() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return width;
}

int Window::getHeight() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return height;
}

glm::ivec2 Window::getSize() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return glm::ivec2(width, height);
}

GLuint& Window::getQuadVAO() {
    return this->quadVAO;
}

const std::string& Window::getTitle() const {
    return this->title;
}

void Window::setTitle(const std::string& newTitle) {
    this->title = newTitle;
    glfwSetWindowTitle(this->window, newTitle.c_str());
}

