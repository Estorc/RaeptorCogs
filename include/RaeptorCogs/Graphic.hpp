/** ********************************************************************************
 * @section Graphic_Overview Overview
 * @file Graphic.hpp
 * @brief High-level 2D graphic handling utilities.
 * @details
 * Typical use cases:
 * - Handling 2D graphics rendering, instance data management, and batching
 * *********************************************************************************
 * @section Graphic_Header Header
 * <RaeptorCogs/Graphic.hpp>
 ***********************************************************************************
 * @section Graphic_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <RaeptorCogs/Flags.hpp>
#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Shape.hpp>
#include <RaeptorCogs/Node.hpp>
#include <RaeptorCogs/GAPI/Common/Core/GraphicHandler.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
namespace RaeptorCogs::GAPI::Common {
    class RendererBackend;
}

namespace RaeptorCogs {

/**
 * @brief Graphic flags enumeration.
 * 
 * Defines flags for graphic state management.
 */
enum class GraphicFlags : uint32_t {
    /** No flags set */
    NONE = 0,
    /** Flag to indicate if the graphic's instance data needs to be rebuilt */
    DATA_DIRTY = 1 << 0,   
    /** Flag to indicate if the graphic should inherit read mask from parent */
    INHERIT_READ_MASK = 1 << 1,
    /** Flag to disable batching for this graphic (disables batch processing for this graphic - used by text) */
    NO_BATCHING = 1 << 2
};

/**
 * @brief Compute instance data mode enumeration.
 * 
 * Defines modes for computing instance data.
 */
enum class ComputeInstanceDataMode {
    /** No instance data computation */
    NONE = 0,
    /** Force rebuild of instance data */
    FORCE_REBUILD = 1,   
    /** Rebuild texture atlas if needed */
    REBUILD_TEXTURE = 2 
};

/**
 * @brief Base Graphic2D class.
 * 
 * Provides an interface for 2D graphic objects.
 * 
 * Typical use cases:
 * - Creating and managing 2D graphics in a rendering engine.
 */
class Graphic2D : public RegisterNode<Graphic2D, Node>, public FlagSet<GraphicFlags> {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the renderer.
         * 
         * Used for rendering operations.
         */
        Singletons::Renderer* renderer = nullptr;

        /**
         * @brief Cursor index in the batch handler.
         * 
         * Indicates the position of this graphic in the batch processing.
         * @see GAPI::Common::GraphicBatchHandler
         */
        size_t batchHandlerCursor = 0;

        /**
         * @brief List of render lists this graphic belongs to.
         * 
         * @see GAPI::Common::RenderList
         */
        std::vector<GAPI::Common::RenderList*> renderLists;

        /**
         * @brief Z-index of the graphic.
         * 
         * Determines the rendering order of the graphic.
         */
        float zIndex = 0.0f;

        /**
         * @brief Reading mask index.
         * 
         * Used for masking operations during rendering.
         */
        int readingMaskIndex = 0;

        /**
         * @brief Writing mask index.
         * 
         * Used for masking operations during rendering.
         */
        int writingMaskIndex = 0;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Graphic2D.
         */
        Graphic2D() {
            FlagSet<GraphicFlags>::setFlag(GraphicFlags::DATA_DIRTY);
            FlagSet<GraphicFlags>::setFlag(GraphicFlags::INHERIT_READ_MASK);
        };

        /**
         * @brief Virtual destructor for Graphic2D.
         */
        virtual ~Graphic2D();

        /**
         * @brief Compute instance data for the graphic.
         * 
         * @param instanceAllocator Reference to the instance allocator.
         * @param mode Mode for computing instance data.
         * @return true if updates were made, false otherwise.
         * 
         * @note This is how GPU side instance data is computed. Override this method in derived classes to provide specific instance data computation.
         */
        virtual bool computeInstanceData(GAPI::Common::InstanceAllocator &instanceAllocator, ComputeInstanceDataMode mode = ComputeInstanceDataMode::NONE);
        
        /**
         * @brief Bind the graphic for rendering.
         * 
         * @note Override this method in derived classes to provide specific binding behavior.
         */
        virtual void bind() const = 0;

        /**
         * @brief Get the unique ID of the graphic.
         * 
         * @return Unique ID of the graphic.
         */
        virtual uint32_t getID() const = 0;

        /**
         * @brief Get the program ID used for rendering.
         * 
         * @return Program ID.
         */
        virtual uint32_t getProgramID() const { return 0; }

        /**
         * @brief Set the renderer for the graphic.
         * 
         * @param renderer Pointer to the renderer.
         * 
         * @note This method is called when the graphic is added to a renderer.
         * @see GAPI::Singletons::Renderer
         */
        virtual void setRenderer(Singletons::Renderer* renderer);

        /**
         * @brief Set the z-index of the graphic.
         * 
         * @param z Z-index value.
         * 
         * @note Higher z-index values are rendered on top of lower values.
         */
        virtual void setZIndex(float z);

        /**
         * @brief Set the reading mask ID.
         * 
         * @param index Reading mask index.
         * @param inheritFromParent Whether to inherit the reading mask from the parent graphic.
         * 
         * @note If inheritFromParent is true, the reading mask will only be applied if no reading mask was already set.
         */
        void setReadingMaskID(int index, bool inheritFromParent = false);

        /**
         * @brief Set the writing mask ID.
         * 
         * @param index Writing mask index.
         */
        void setWritingMaskID(int index);

        /**
         * @brief Set the batch handler cursor index.
         * 
         * @param cursor Cursor index.
         * 
         * @note This index indicates the position of this graphic in the batch processing.
         * @see GAPI::Common::GraphicBatchHandler
         */
        void setBatchHandlerCursor(size_t cursor) { this->batchHandlerCursor = cursor; }

        /**
         * @brief Check if the graphic is visible.
         * 
         * @return true if the graphic is visible, false otherwise.
         * 
         * @note Override this method in derived classes to provide specific visibility behavior.
         */
        virtual bool isVisible() const = 0;

        /**
         * @brief Check if the graphic is opaque.
         * 
         * @return true if the graphic is opaque, false otherwise.
         * 
         * @note Override this method in derived classes to provide specific opacity behavior.
         */
        virtual bool isOpaque() const = 0;

        /**
         * @brief Get the z-index of the graphic.
         * 
         * @return Z-index value.
         * 
         * @note Higher z-index values are rendered on top of lower values.
         */
        float getZIndex() const;

        /**
         * @brief Get the writing mask ID.
         * 
         * @return Writing mask index.
         * 
         * @note A value of 0 indicates no writing mask is applied.
         */
        int getWritingMaskID() const;

        /**
         * @brief Get the reading mask ID.
         * 
         * @return Reading mask index.
         */
        int getReadingMaskID() const;

        /**
         * @brief Get the texture associated with the graphic.
         * 
         * @return Texture object.
         * 
         * @note Override this method in derived classes to provide specific texture retrieval behavior.
         */
        virtual Texture getTexture() const { return nullptr; }

        /**
         * @brief Get the batch handler cursor index.
         * 
         * @return Cursor index.
         * 
         * @note This index indicates the position of this graphic in the batch processing.
         * @see GAPI::Common::GraphicBatchHandler
         */
        size_t getBatchHandlerCursor() const { return this->batchHandlerCursor; }

        /**
         * @brief Get the number of render lists this graphic belongs to.
         * 
         * @return Number of render lists.
         */
        size_t getRenderListCount() const { return this->renderLists.size(); }

        /**
         * @brief Get the render lists this graphic belongs to.
         * 
         * @return Vector of render list pointers.
         */
        std::vector<GAPI::Common::RenderList*>& getRenderLists() { return this->renderLists; }

        /**
         * @brief Get the batch handler associated with this graphic.
         * 
         * @return Reference to the GraphicBatchHandler.
         */
        GAPI::Common::GraphicBatchHandler &getBatchHandler();

        /**
         * @brief Get the renderer associated with this graphic.
         * 
         * @return Pointer to the Renderer.
         */
        Singletons::Renderer* getRenderer() const;

        /**
         * @brief Build the renderer key for batching.
         * 
         * @return BatchKey object.
         * 
         * @note The renderer key is used for ordering and batching graphics during rendering.
         */
        BatchKey buildRendererKey() {
            return BatchKey{
                !!this->getWritingMaskID(),
                this->getWritingMaskID() ? this->getReadingMaskID() : 0,
                static_cast<int>(this->getWritingMaskID() ? -this->getZIndex() : this->getZIndex()),
                this->getWritingMaskID() ? false : this->isOpaque(),
                /** TODO: For now program is always the same. Update when multiple programs are supported. */
                this->getProgramID(),
                this->getID()
            };
        }

        /**
         * @brief Set the data dirty flag.
         * 
         * @param dirty Boolean indicating if the data is dirty.
         * 
         * @note When data is dirty, instance data needs to be recomputed.
         */
        void setDataDirty(bool dirty);

        /**
         * @brief Check if the data is dirty.
         * 
         * @return true if the data is dirty, false otherwise.
         * 
         * @note When data is dirty, instance data needs to be recomputed.
         */
        bool isDataDirty() const;

        /**
         * @brief Set the parent node of the graphic.
         * 
         * @param parent Pointer to the parent node.
         * 
         * @note This method overrides the base class implementation to handle graphic-specific parent setting.
         */
        void setParent(Node* parent) override;

        /**
         * @brief Update the graphic's position in the render lists.
         * 
         * @note Called when z-index or mask IDs change.
         * @see GAPI::Common::RenderList
         */
        void updatePositionInRenderLists();
};

/**
 * @brief RenderableGraphicFlags enumeration.
 * 
 * Defines flags for renderable graphic state management.
 */
enum class RenderableGraphicFlags : uint32_t {
    /** No flags set */
    NONE = 0,
    /** Flag to indicate if the graphic's instance data needs to be rebuilt */
    GLOBAL_COLOR_DIRTY = 1 << 0,
    /** Flag to indicate if the graphic is visible */
    IS_VISIBLE = 1 << 1
};

class RenderableGraphic2D : public RegisterNode<RenderableGraphic2D, Graphic2D>, public FlagSet<RenderableGraphicFlags> {
    private:
        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Color of the graphic.
         * 
         * Used for tinting the graphic.
         */
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        /**
         * @brief Global color of the graphic.
         * 
         * Computed by combining the graphic's color with its parent's global color.
         */
        glm::vec3 globalColor = glm::vec3(1.0f, 1.0f, 1.0f);

        /**
         * @brief Shape of the graphic.
         * 
         * Defines the geometry of the graphic.
         */
        std::shared_ptr<Shape> shape = std::make_shared<Quad>();

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for RenderableGraphic2D.
         * 
         * Initializes the graphic as visible and marks the global color as dirty.
         */
        RenderableGraphic2D() {
            FlagSet<RenderableGraphicFlags>::setFlag(RenderableGraphicFlags::IS_VISIBLE);
            FlagSet<RenderableGraphicFlags>::setFlag(RenderableGraphicFlags::GLOBAL_COLOR_DIRTY);
        };

        /**
         * @brief Destructor for RenderableGraphic2D.
         */
        ~RenderableGraphic2D() = default;

        /**
         * @brief Set the color of the graphic.
         * 
         * @param color Color vector (RGB).
         * 
         * @note Setting the color marks the data as dirty.
         */
        void setColor(const glm::vec3 &color);

        /**
         * @brief Set the shape of the graphic.
         * 
         * @tparam ShapeType Type of the shape (must derive from Shape).
         * @param args Arguments to construct the shape.
         * 
         * @note Setting the shape marks the data as dirty.
         */
        template<typename ShapeType, typename... Args>
        void setShape(Args&&... args) {
            static_assert(std::is_base_of<Shape, ShapeType>::value,
                        "ShapeType must derive from Shape");
            static_assert(std::is_constructible<ShapeType, Args...>::value,
                        "Provided arguments cannot be used to construct ShapeType");
            shape = std::make_unique<ShapeType>(std::forward<Args>(args)...);
            this->setDataDirty(true);
        }

        /**
         * @brief Get the shape of the graphic.
         * 
         * @return Reference to the shape.
         */
        Shape &getShape() const;

        /**
         * @brief Get the color of the graphic.
         * 
         * @return Color vector (RGB).
         */
        glm::vec3 getColor() const;

        /**
         * @brief Rebuild the global color of the graphic.
         * 
         * @note The global color is computed by combining the graphic's color with its parent's global color.
         */
        void rebuildGlobalColor();

        /**
         * @brief Get the global color of the graphic.
         * 
         * @return Global color vector (RGB).
         */
        glm::vec3 getGlobalColor();

        /**
         * @brief Set the visibility of the graphic.
         * 
         * @param visible Boolean indicating if the graphic should be visible.
         */
        void setVisibility(bool visible);

        /**
         * @brief Check if the graphic is visible.
         * 
         * @return True if visible, false otherwise.
         */
        bool isVisible() const override;

        /**
         * @brief Set the global color dirty flag.
         * 
         * @param dirty Boolean indicating if the global color is dirty.
         */
        void setGlobalColorDirty(bool dirty);

        /**
         * @brief Check if the global color is dirty.
         * 
         * @return True if the global color is dirty, false otherwise.
         */
        bool isGlobalColorDirty() const;

        /**
         * @brief Set the parent node of the graphic.
         * 
         * @param parent Pointer to the parent node.
         * 
         * @note This method overrides the base class implementation to handle graphic-specific parent setting.
         */
        void setParent(Node* parent) override;
};

/**
 * @brief Transform flags enumeration.
 * 
 * Defines flags for transformable graphic state management.
 */
enum class TransformFlags : uint32_t {
    /** No flags set */
    NONE = 0,
    /** Flag to indicate if the local transformation matrix needs to be rebuilt */
    LOCAL_MATRIX_DIRTY = 1 << 0,
    /** Flag to indicate if the global transformation matrix needs to be rebuilt */
    GLOBAL_MATRIX_DIRTY = 1 << 1
};

/**
 * @brief TransformableGraphic2D class.
 * 
 * Provides functionality for 2D graphics that can be transformed.
 * 
 * Typical use cases:
 * - Managing position, size, scale, rotation, and anchor point of 2D graphics
 * 
 * @see Graphic2D
 */
class TransformableGraphic2D : public RegisterNode<TransformableGraphic2D, RenderableGraphic2D>, public FlagSet<TransformFlags> {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Local transformation matrix.
         * 
         * Used to represent the graphic transform in its local space.
         */
        glm::mat4 localMatrix = glm::mat4(1.0f);

        /**
         * @brief Global transformation matrix.
         * 
         * Used to represent the graphic transform in world space.
         */
        glm::mat4 globalMatrix = glm::mat4(1.0f);

        /**
         * @brief Position of the graphic.
         * 
         * Defines the position in 2D space.
         */
        glm::vec2 position = glm::vec2(0.0f, 0.0f);

        /**
         * @brief Size of the graphic.
         * 
         * Defines the width and height.
         */
        glm::vec2 size = glm::vec2(1.0f, 1.0f);

        /**
         * @brief Scale of the graphic.
         * 
         * Defines the scaling factors along x and y axes.
         */
        glm::vec2 scale = glm::vec2(1.0f, 1.0f);

        /**
         * @brief Rotation of the graphic.
         * 
         * Defines the rotation angle in radians.
         */
        float rotation = 0.0f;

        /**
         * @brief Anchor point of the graphic.
         * 
         * Defines the pivot point for transformations.
         */
        glm::vec2 anchor = glm::vec2(0.0f, 0.0f);
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for TransformableGraphic2D.
         * 
         * Initializes the local and global matrices as dirty.
         */
        TransformableGraphic2D() {
            FlagSet<TransformFlags>::setFlag(TransformFlags::LOCAL_MATRIX_DIRTY);
            FlagSet<TransformFlags>::setFlag(TransformFlags::GLOBAL_MATRIX_DIRTY);
        };

        /**
         * @brief Destructor for TransformableGraphic2D.
         */
        ~TransformableGraphic2D() = default;

        /**
         * @brief Rebuild the local transformation matrix.
         * 
         * @note This method recalculates the local matrix based on position, size, scale, rotation, and anchor point.
         */
        void rebuildLocalMatrix();

        /**
         * @brief Rebuild the global transformation matrix.
         * 
         * @note This method recalculates the global matrix by combining the local matrix with the parent's global matrix.
         */
        void rebuildGlobalMatrix();

        /**
         * @brief Get the position of the graphic.
         * 
         * @return position of the graphic
         */
        glm::vec2 getPosition() const;

        /**
         * @brief Get the size of the graphic.
         * 
         * @return size of the graphic
         */
        glm::vec2 getSize() const;

        /**
         * @brief Get the scale of the graphic.
         * 
         * @return scale of the graphic
         */
        glm::vec2 getScale() const;

        /**
         * @brief Get the rotation of the graphic.
         * 
         * @return rotation angle in radians
         */
        float getRotation() const;

        /**
         * @brief Get the anchor point of the graphic.
         * 
         * @return anchor point of the graphic
         */
        glm::vec2 getAnchor() const;

        /**
         * @brief Set the position of the graphic.
         * 
         * @param pos New position vector
         * 
         * @note Setting the position marks the local and global matrices as dirty.
         */
        void setPosition(const glm::vec2 &pos);

        /**
         * @brief Set the size of the graphic.
         * 
         * @param size New size vector
         * 
         * @note Setting the size marks the local and global matrices as dirty.
         */
        void setSize(const glm::vec2 &size);

        /**
         * @brief Set the scale of the graphic.
         * 
         * @param scale New scale vector
         * 
         * @note Setting the scale marks the local and global matrices as dirty.
         */
        void setScale(const glm::vec2 &scale);

        /**
         * @brief Set the rotation of the graphic.
         * 
         * @param angle New rotation angle in radians
         * 
         * @note Setting the rotation marks the local and global matrices as dirty.
         */
        void setRotation(float angle);

        /**
         * @brief Set the anchor point of the graphic.
         * 
         * @param anchor New anchor point vector
         * 
         * @note Setting the anchor point marks the local and global matrices as dirty.
         */
        void setAnchor(const glm::vec2 &anchor);

        /**
         * @brief Set the z-index of the graphic.
         * 
         * @param z Z-index value.
         * 
         * @note Setting the z-index marks the local matrix as dirty.
         */
        void setZIndex(float z) override;

        /**
         * @brief Set the parent node of the graphic.
         * 
         * @param parent Pointer to the parent node.
         * 
         * @note This method overrides the base class implementation to handle graphic-specific parent setting.
         */
        void setParent(Node* parent) override;

        /**
         * @brief Get the model matrix of the graphic.
         * 
         * @return Model matrix (global transformation matrix).
         * 
         * @note The model matrix represents the graphic's transformation in world space.
         */
        glm::mat4 getModelMatrix();

        /**
         * @brief Get the local transformation matrix.
         * 
         * @return Reference to the local transformation matrix.
         */
        glm::mat4 &getLocalMatrix();

        /**
         * @brief Get the global transformation matrix.
         * 
         * @return Reference to the global transformation matrix.
         */
        glm::mat4 &getGlobalMatrix();

        /**
         * @brief Set the local matrix dirty flag.
         * 
         * @param dirty New state of the local matrix dirty flag.
         */
        void setLocalMatrixDirty(bool dirty);

        /**
         * @brief Set the global matrix dirty flag.
         * 
         * @param dirty New state of the global matrix dirty flag.
         */
        void setGlobalMatrixDirty(bool dirty);

        /**
         * @brief Check if the local matrix is dirty.
         * 
         * @return true if the local matrix is dirty, false otherwise.
         */
        bool isLocalMatrixDirty() const;

        /**
         * @brief Check if the global matrix is dirty.
         * 
         * @return true if the global matrix is dirty, false otherwise.
         */
        bool isGlobalMatrixDirty() const;
};

}