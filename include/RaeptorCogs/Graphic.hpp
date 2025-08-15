#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/Renderer.hpp>
namespace RaeptorCogs::Singletons {
class Renderer;
}

namespace RaeptorCogs {

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
        Singletons::Renderer* renderer = nullptr;
        BatchKey rendererKey = std::make_tuple(0, true, 0, 0); // Default key with z-index 0, opaque, and no texture
        float zIndex = 0.0f;
    public:
        virtual ~Graphic();

        virtual void computeInstanceData(InstanceData &data, std::vector<float> &instanceDataBuffer) {};
        virtual void bind() const {};
        virtual GLuint getID() const {return 0;};
        virtual GLuint getProgramID() const {return 0;};

        virtual void setRenderer(Singletons::Renderer* renderer);
        virtual void setRendererKey(BatchKey key);
        virtual void setZIndex(float z);

        virtual bool isVisible() const { return true; }
        virtual bool isOpaque() const { return true; }
        virtual float getZIndex() const;
    
        virtual Singletons::Renderer* getRenderer() const;
        virtual BatchKey getRendererKey() const;
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
        glm::vec3 color;

        GraphicFlags flags;

    public:
        TransformableGraphic2D() : position(0.0f), size(1.0f), rotation(0.0f), anchor(0.0f), modelMatrix(1.0f), color(1.0f, 1.0f, 1.0f) {
            flags = GraphicFlags::NEEDS_REBUILD | GraphicFlags::IS_VISIBLE;
        }
        virtual void rebuild();

        virtual void setPosition(const glm::vec2 &pos);
        virtual void setZIndex(float z) override;
        virtual void setSize(const glm::vec2 &size);
        virtual void setRotation(float angle);
        virtual void setColor(const glm::vec3 &color);
        virtual void setAnchor(const glm::vec2 &anchor);
        virtual void setVisibility(bool visible);

        virtual bool isVisible() const override;
        virtual bool needsRebuild() const;

        virtual glm::vec2 getPosition() const;

        virtual glm::vec2 getSize() const;
        virtual float getRotation() const;
        virtual glm::vec3 getColor() const;
        virtual glm::vec2 getAnchor() const;
        virtual glm::mat4 getModelMatrix();
};

}