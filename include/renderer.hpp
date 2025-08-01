#pragma once
#include <main.hpp>
#include <unordered_map>
#include <sprite.hpp>
#include <graphic.hpp>
#include <memory>
#include <flags.hpp>
#include <constants.glsl>

class RaeptorApplication;
class Sprite;
class Renderer;
class Graphic;

typedef int RendererMode;

class Renderer {
    private:
        RaeptorApplication* app = nullptr;
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
        Renderer(RaeptorApplication* app);
        ~Renderer();

        void render();

        void addGraphic(Graphic*);
        void changeGraphicPosition(Graphic* graphic, GLuint newTextureID);
        void removeGraphic(Graphic*);
        void clearBatches();

        void setMode(RendererMode mode);

        RendererMode getMode() const;
};