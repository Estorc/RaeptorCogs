#pragma once
#include <unordered_map>
#include <map>
#include <memory>
#include <RaeptorLab/flags.hpp>
#include <RaeptorLab/window.hpp>
#include "../../shaders/constants.glsl"
#include <GLFW/glfw3.h>

class Sprite;
class Renderer;
class Graphic;

using BatchKey = std::tuple<int /*zindex*/, bool /*isOpaque*/, GLuint /*textureID*/>;
using BatchVector = std::vector<Graphic*>;

class Renderer {
    private:
        std::map<BatchKey, BatchVector> batches; // Maps batch keys to graphics
        GLuint shader = 0;
        GLuint quadVBO;
        GLuint quadEBO;
        GLuint instanceVBO;
        GLuint instanceSSBO;
        Window *mainWindow;
        std::vector<uint8_t> instanceDataBuffer; // Buffer to hold instance data
    public:
        Window* initialize(int width, int height, const char* title);
        void destroy();

        Window* initGraphics(int width, int height, const char* title);
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
};