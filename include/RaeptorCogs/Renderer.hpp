#pragma once
#include <unordered_map>
#include <map>
#include <memory>
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/Window.hpp>
#include "../../shaders/constants.glsl"
#include <GLFW/glfw3.h>
#ifdef USE_IMGUI
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#endif

namespace RaeptorCogs {

class Graphic;
using BatchKey = std::tuple<int /*zindex*/, bool /*isOpaque*/, GLuint /*programID*/, GLuint /*textureID*/>;
using BatchVector = std::vector<Graphic*>;

}

namespace RaeptorCogs::Singletons {

class Renderer {
    private:
        std::vector<Window*> windows;
        std::map<BatchKey, BatchVector> batches; // Maps batch keys to graphics
        GLuint shader = 0;
        GLuint quadVBO;
        GLuint quadEBO;
        GLuint instanceVBO;
        #ifdef __EMSCRIPTEN__
        GLuint iDataTex;
        #else
        GLuint instanceSSBO;
        #endif
        std::vector<float> instanceDataBuffer; // Buffer to hold instance data
    public:
        Window* initialize(int width = 0, int height = 0, const char* title = nullptr);
        ~Renderer();

        Window* initGraphics(int width = 0, int height = 0, const char* title = nullptr);
        void buildBuffers();
        void buildShader();
        void buildVAO(Window* window);
        void render(Window* window, int width = 0, int height = 0);

        Window* createWindow(int width, int height, const char* title);
        void destroyWindow(Window* window);

        void addGraphic(Graphic*);
        void changeGraphicPosition(Graphic* graphic);
        void removeGraphic(Graphic*);
        void clearBatches();

        void NewImGuiFrame();
};
}