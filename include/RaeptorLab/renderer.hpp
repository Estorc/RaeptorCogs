#pragma once
#include <unordered_map>
#include <RaeptorLab/graphic.hpp>
#include <memory>
#include <RaeptorLab/flags.hpp>
#include <RaeptorLab/window.hpp>
#include <constants.glsl>
#include <GLFW/glfw3.h>

class Sprite;
class Renderer;
class Graphic;

class Renderer {
    private:
        std::unordered_map<GLuint, std::vector<Graphic*>> batches; // Maps texture IDs to graphics
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
        void render(Window* window);

        Window* createWindow(int width, int height, const char* title);
        void destroyWindow(Window* window);

        void addGraphic(Graphic*);
        void changeGraphicPosition(Graphic* graphic, GLuint newTextureID);
        void removeGraphic(Graphic*);
        void clearBatches();
};