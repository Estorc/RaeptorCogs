#pragma once
#include <unordered_map>
#include <RaeptorLab/graphic.hpp>
#include <memory>
#include <RaeptorLab/flags.hpp>
#include <constants.glsl>
#include <GLFW/glfw3.h>

class Sprite;
class Renderer;
class Graphic;

typedef int RendererMode;

class Renderer {
    private:
        std::unordered_map<GLuint, std::vector<Graphic*>> batches; // Maps texture IDs to graphics
        GLuint shader = 0;
        GLuint quadVBO;
        GLuint quadEBO;
        GLuint quadVAO;
        GLuint instanceVBO;
        GLuint instanceSSBO;
        RendererMode mode = RENDERER_MODE_2D_SPRITE;
        std::vector<uint8_t> instanceDataBuffer; // Buffer to hold instance data
    public:
        Renderer();
        ~Renderer();

        void render(GLFWwindow* window);

        void addGraphic(Graphic*);
        void changeGraphicPosition(Graphic* graphic, GLuint newTextureID);
        void removeGraphic(Graphic*);
        void clearBatches();

        void setMode(RendererMode mode);

        RendererMode getMode() const;
};