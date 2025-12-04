#include <RaeptorCogs/Window.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <iostream>
#include <RaeptorCogs/External/stb/stb.hpp>

#include <RaeptorCogs/External/glad/glad.hpp>
#include <GLFW/glfw3.h>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace RaeptorCogs {


#ifdef __EMSCRIPTEN__
EM_BOOL resize_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {

    RaeptorCogs::Window* window = static_cast<RaeptorCogs::Window*>(userData);

    int new_width = uiEvent->windowInnerWidth;
    int new_height = uiEvent->windowInnerHeight;

    printf("Window resized: %d x %d\n", new_width, new_height);

    int width = new_width;
    int height = new_height;
    #ifdef __EMSCRIPTEN__
    double dpr = emscripten_get_device_pixel_ratio();
    width = (int)round((double)width * dpr);
    height = (int)round((double)height * dpr);
    #endif
    window->setSize(width, height);
    #ifdef __EMSCRIPTEN__
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(new_width, new_height);
    io.DisplayFramebufferScale = ImVec2((float)dpr, (float)dpr);
    // set CSS size (in CSS pixels)
    emscripten_set_element_css_size("#canvas", new_width, new_height);
    // set backing size to CSS * DPR (in device pixels)
    emscripten_set_canvas_element_size("#canvas", width, height);
    // then call glViewport(0,0,backing_w,backing_h) in your GL code
    #endif

    return EM_TRUE; // event handled
}
#endif


Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor, GLFWwindow* share, RaeptorCogs::GAPI::Common::WindowContext* context, WindowFlags flags) 
    : title(title), context(context) {

    #ifdef __EMSCRIPTEN__
    int frameWidth = GetScreenWidth();
    int frameHeight = GetScreenHeight();
    double dpr = emscripten_get_device_pixel_ratio();
    width = (int)round((double)GetScreenWidth() * dpr);
    height = (int)round((double)GetScreenHeight() * dpr);
    #endif

    this->setFlag(flags);
    this->window = glfwCreateWindow(width, height, title.c_str(), monitor, share);
    if (!this->window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
    }
    if (context)
        context->initialize(this);
    Renderer().getBackend().getImGuiModule().createImGuiContext(this->imGuiContext, this->window);
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_element_css_size("#canvas", frameWidth, frameHeight);
    emscripten_set_canvas_element_size("#canvas", width, height);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, EM_TRUE, resize_callback);
    #endif
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Window::~Window() {
    if (this->window) {
        glfwDestroyWindow(this->window);
    }
    if (this->context) {
        delete this->context;
    }
    for (const auto& [shape, cursor] : this->cursors) {
        if (cursor) {
            glfwDestroyCursor(cursor);
        }
    }
}

GLFWwindow* Window::getGLFWWindow() const {
    return this->window;
}

unsigned int Window::getWidth() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return static_cast<unsigned int>(width);
}

unsigned int Window::getHeight() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return static_cast<unsigned int>(height);
}

glm::ivec2 Window::getSize() const {
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);
    return glm::ivec2(width, height);
}

glm::ivec2 Window::getMousePosition() const {
    double xpos, ypos;
    glfwGetCursorPos(this->window, &xpos, &ypos);
    return glm::ivec2(static_cast<int>(xpos), static_cast<int>(ypos));
}

bool Window::isMouseInWindow() const {
    glm::ivec2 mousePos = this->getMousePosition();
    glm::ivec2 windowSize = this->getSize();
    return (mousePos.x >= 0 && mousePos.x < windowSize.x && mousePos.y >= 0 && mousePos.y < windowSize.y);
}

const std::string& Window::getTitle() const {
    return this->title;
}

RaeptorCogs::GAPI::Common::WindowContext* Window::getContext() const {
    return this->context;
}



void Window::makeContextCurrent() {
    glfwMakeContextCurrent(this->window);
    #ifdef USE_IMGUI
    ImGui::SetCurrentContext(this->imGuiContext);
    #endif
    this->context->bind();
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
    glfwSetWindowIcon(window, static_cast<int>(icon.size()), icon.data());

    // free memory
    for (const auto& img : icon) {
        stbi_image_free(img.pixels);
    }
}

void Window::selectCursor(int cursorShape) {
    if (this->cursors.find(cursorShape) == this->cursors.end()) {
        GLFWcursor* cursor = glfwCreateStandardCursor(cursorShape);
        if (cursor) {
            this->cursors[cursorShape] = cursor;
        } else {
            std::cerr << "Failed to create cursor of shape: " << cursorShape << std::endl;
            return;
        }
    }
    glfwSetCursor(this->window, this->cursors[cursorShape]);
}

}