#include <RaeptorCogs/RaeptorCogs.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <nfd.h>
#endif

#ifdef __EMSCRIPTEN__
EM_JS(double, _GetScreenWidthJS, (), {
    return window.innerWidth;
});

// JavaScript function to get window height
EM_JS(double, _GetScreenHeightJS, (), {
    return window.innerHeight;
});
#endif

namespace RaeptorCogs {

void Initialize() {
    LocalizeWorkingDirectory();
    #ifndef __EMSCRIPTEN__
    NFD_Init();
    #endif
}

#ifdef __EMSCRIPTEN__
static std::function<void()> g_updateFunction;
void emscripten_loop_wrapper() {
    g_updateFunction();
    glfwPollEvents();
}
#endif

void StartLoop(std::function<void()> updateFunction, Window* window) {
    #ifndef __EMSCRIPTEN__
    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        updateFunction();
        glfwPollEvents();
    }
    #else
    g_updateFunction = updateFunction;
    emscripten_set_main_loop(emscripten_loop_wrapper, 0, 1);
    #endif
}

void Destroy() {
    #ifndef __EMSCRIPTEN__
    NFD_Quit();
    #endif
}

#ifdef __EMSCRIPTEN__
double GetScreenWidth() {
    return _GetScreenWidthJS();
}

double GetScreenHeight() {
    return _GetScreenHeightJS();
}
#else
double GetScreenWidth() {
    return static_cast<double>(glfwGetVideoMode(glfwGetPrimaryMonitor())->width);
}

double GetScreenHeight() {
    return static_cast<double>(glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
}
#endif


Singletons::Renderer& RaeptorCogs::Renderer() {
    static Singletons::Renderer instance;
    return instance;
}

Singletons::TextureAtlasManager& RaeptorCogs::TextureAtlasManager() {
    static Singletons::TextureAtlasManager instance;
    return instance;
}

}