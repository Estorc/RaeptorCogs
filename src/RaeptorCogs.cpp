#include <RaeptorCogs/RaeptorCogs.hpp>
#include <GLFW/glfw3.h>
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

static std::atomic<bool> _exiting = false;
static void exitHandler() {
    _exiting.store(true);
}

bool IsExiting() {
    return _exiting.load();
}

void Initialize() {
    std::atexit(exitHandler);
    LocalizeWorkingDirectory();
    #ifndef __EMSCRIPTEN__
    NFD_Init();
    #endif
}

void MainLoop(std::function<void(Window&)> updateFunction, Window &window) {
    MainWorker().executeJobs();
    Renderer().getBackend().getRenderPipeline().beginFrame();
    updateFunction(window);
    Renderer().getBackend().getRenderPipeline().endFrame();
    glfwPollEvents();
    Input().update(window);
    Mouse().update(window);
    Time().computeDeltaTime();
}

#ifdef __EMSCRIPTEN__
static std::function<void(Window&)> g_updateFunction;
Window* g_window = nullptr;
void emscripten_loop_wrapper() {
    MainLoop(g_updateFunction, g_window);
}
#endif

void StartLoop(std::function<void(Window&)> updateFunction, Window &window) {
    glfwSetScrollCallback(window.getGLFWWindow(), [](GLFWwindow*, double xoffset, double yoffset) {
        RaeptorCogs::Mouse().updateScroll(xoffset, yoffset);
    });
    #ifndef __EMSCRIPTEN__
    while (!glfwWindowShouldClose(window.getGLFWWindow())) {
        MainLoop(updateFunction, window);
    }
    #else
    g_updateFunction = updateFunction;
    g_window = &window;
    emscripten_set_main_loop(emscripten_loop_wrapper, 0, 1);
    #endif
}

void Destroy() {
    ResourceWorker().stop();
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


Singletons::Renderer& Renderer() {
    return RaeptorCogs::SingletonAccessor<Singletons::Renderer>::get();
}

Singletons::TextureAtlasManager& TextureAtlasManager() {
    return RaeptorCogs::SingletonAccessor<Singletons::TextureAtlasManager>::get();
}

Singletons::Input& Input() {
    return RaeptorCogs::SingletonAccessor<Singletons::Input>::get();
}

Singletons::Mouse& Mouse() {
    return RaeptorCogs::SingletonAccessor<Singletons::Mouse>::get();
}

Singletons::Time& Time() {
    return RaeptorCogs::SingletonAccessor<Singletons::Time>::get();
}

Singletons::Random& Random() {
    return RaeptorCogs::SingletonAccessor<Singletons::Random>::get();
}

Singletons::Platform& Platform() {
    return RaeptorCogs::SingletonAccessor<Singletons::Platform>::get();
}


#pragma region Workers

Singletons::MainWorker& MainWorker() {
    return RaeptorCogs::SingletonAccessor<Singletons::MainWorker>::get();
}

Worker& ResourceWorker() {
    return RaeptorCogs::SingletonAccessor<Worker>::get();
}

#pragma endregion

}