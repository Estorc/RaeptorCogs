#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <flags.hpp>

class Renderer;

struct InstanceData {
    glm::mat4 model;
    glm::vec4 uvRect;
    uint64_t type;         // 0 = Static, 1 = Animated...
    uint64_t dataOffset;   // Offset into another SSBO or same buffer
};

enum class GraphicFlags : uint32_t {
    NONE = 0,
    NEEDS_REBUILD = 1 << 0, // Flag to indicate if the graphic needs to be rebuilt
    IS_VISIBLE = 1 << 1 // Flag to indicate if the graphic is visible
};
template<>
struct EnableBitmaskOperators<GraphicFlags> {
    static constexpr bool enable = true;
};

// Proxy class for Renderer to manage rendering of various graphics
class Graphic {
    protected:
        Renderer* renderer = nullptr;
        GLuint rendererKey = 0; // Unique key for renderer to identify the graphic
    public:
        virtual ~Graphic();

        virtual void prepareRender(Renderer* renderer) {};
        virtual void computeInstanceData(InstanceData &data, std::vector<uint8_t> &instanceDataBuffer) {};
        virtual void bind() const {};
        virtual GLuint getID() const {return 0;};

        virtual void setRenderer(Renderer* renderer);
        virtual void setRendererKey(GLuint key);

        virtual Renderer* getRenderer() const;
        virtual GLuint getRendererKey() const;
};

class Graphic2D : public Graphic {
    // Base class for 2D graphics
    // Placeholder for now, can be extended later
};

class TransformableGraphic2D : public Graphic {
    protected:
        glm::vec2 position;
        glm::vec2 size;
        float rotation;
        glm::vec2 anchor;
        glm::mat4 modelMatrix;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); // Default color is white

        GraphicFlags flags;

    public:
        virtual void rebuild();

        virtual void setPosition(const glm::vec2 &pos);
        virtual void setSize(const glm::vec2 &size);
        virtual void setRotation(float angle);
        virtual void setColor(const glm::vec3 &color);
        virtual void setAnchor(const glm::vec2 &anchor);
        virtual void setVisibility(bool visible);

        virtual bool isVisible() const;
        virtual bool needsRebuild() const;

        virtual glm::vec2 getPosition() const;
        virtual glm::vec2 getSize() const;
        virtual float getRotation() const;
        virtual glm::vec3 getColor() const;
        virtual glm::vec2 getAnchor() const;
        virtual glm::mat4 getModelMatrix();
};