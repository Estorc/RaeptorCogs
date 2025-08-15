#include <RaeptorCogs/Window.hpp>
#include <iostream>
#include <stb_image.h>

namespace RaeptorCogs {

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

void Window::setSize(int width, int height) {
    glfwSetWindowSize(this->window, width, height);
}

void Window::setIcon(const std::vector<std::string>& iconPaths) {
    std::vector<GLFWimage> icon;
    icon.reserve(iconPaths.size());
    for (const auto& path : iconPaths) {
        GLFWimage img;
        img.pixels = stbi_load(path.c_str(), &img.width, &img.height, 0, 4);
        if (!img.pixels) {
            std::cerr << "Failed to load icon: " << path << std::endl;
            continue;
        }
        icon.push_back(img);
    }

    // set icons
    glfwSetWindowIcon(window, icon.size(), icon.data());

    // free memory
    for (const auto& img : icon) {
        stbi_image_free(img.pixels);
    }
}

}