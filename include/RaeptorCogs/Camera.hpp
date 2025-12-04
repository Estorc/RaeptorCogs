/** ********************************************************************************
 * @section Camera_Overview Overview
 * @file Camera.hpp
 * @brief High-level camera utilities.
 * @details
 * Typical use cases:
 * - Handling 2D camera view and projection matrices
 * *********************************************************************************
 * @section Camera_Header Header
 * <RaeptorCogs/Camera.hpp>
 ***********************************************************************************
 * @section Camera_Metadata Metadata
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
#include <RaeptorCogs/Component.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>


namespace RaeptorCogs {

/**
 * @brief Camera flags enumeration.
 * 
 * Defines flags for camera state management.
 */
enum class CameraFlags : uint32_t {
    /** No flags set. */
    NONE = 0,
    /** Indicates the view matrix needs to be rebuilt. */
    NEEDS_REBUILD_VIEW_MATRIX = 1 << 0,
    /** Indicates the projection matrix needs to be rebuilt. */
    NEEDS_REBUILD_PROJECTION_MATRIX = 1 << 1
};

/**
 * @brief Base Camera class.
 * 
 * Provides an interface for 2D and 3D camera implementations.
 */
class Camera {
    protected:

        // ============================================================================
        //                               PROTECTED ATTRIBUTES
        // ============================================================================

        /** 
         * @brief View matrix of the camera.
         */
        glm::mat4 viewMatrix = glm::mat4(1.0f);

        /**
         * @brief Projection matrix of the camera.
         */
        glm::mat4 projectionMatrix = glm::mat4(1.0f);

        /**
         * @brief Flags indicating the state of the camera.
         */
        CameraFlags flags = CameraFlags::NEEDS_REBUILD_VIEW_MATRIX | CameraFlags::NEEDS_REBUILD_PROJECTION_MATRIX;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Virtual destructor for Camera.
         */
        virtual ~Camera() = default;

        /**
         * @brief Get the view matrix of the camera.
         */
        virtual glm::mat4 getViewMatrix() = 0;

        /**
         * @brief Get the projection matrix of the camera.
         */
        virtual glm::mat4 getProjectionMatrix() = 0;
};

/**
 * @brief 2D Camera class.
 * 
 * Implements a simple 2D camera with position and zoom.
 */
class Camera2D : public Camera, public Component2D {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Position of the camera in 2D space.
         */
        glm::vec2 position = glm::vec2(0.0f, 0.0f);

        /**
         * @brief Zoom level of the camera.
         */
        float zoom = 1.0f;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Rebuild the view matrix based on position and zoom.
         */
        void rebuildViewMatrix();

        /**
         * @brief Rebuild the projection matrix based on zoom.
         */
        void rebuildProjectionMatrix();

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Constructor for Camera2D.
         */
        Camera2D();

        /**
         * @brief Destructor for Camera2D.
         */
        ~Camera2D();

        /**
         * @brief Get the view matrix of the camera.
         */
        glm::mat4 getViewMatrix() override;

        /**
         * @brief Get the projection matrix of the camera.
         */
        glm::mat4 getProjectionMatrix() override;

        /**
         * @brief Set the position of the camera.
         */
        void setPosition(const glm::vec2 &pos);

        /**
         * @brief Set the zoom level of the camera.
         */
        void setZoom(float z);

        /**
         * @brief Get the X position of the camera.
         */
        float getPositionX() const;

        /**
         * @brief Get the Y position of the camera.
         */
        float getPositionY() const;

        /**
         * @brief Get the position of the camera as a vector.
         */
        glm::vec2 getPosition() const;

        /**
         * @brief Get the zoom level of the camera.
         */
        float getZoom() const;

        /**
         * @brief Update the camera component.
         */
        void update(GAPI::Common::RenderPipeline& pipeline) override;
};

}