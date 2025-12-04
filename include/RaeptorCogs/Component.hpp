/** ********************************************************************************
 * @section Component_Overview Overview
 * @file Component.hpp
 * @brief High-level component management utilities.
 * @details
 * Typical use cases:
 * - Creating reusable components for renderer pipelines.
 * *********************************************************************************
 * @section Component_Header Header
 * <RaeptorCogs/Component.hpp>
 ***********************************************************************************
 * @section Component_Metadata Metadata
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
#include <RaeptorCogs/Node.hpp>

namespace RaeptorCogs {


/**
 * @brief Base Component class.
 * 
 * Provides an interface for components that can be attached to nodes.
 * 
 * Typical use cases:
 * - Creating reusable components for renderer pipelines.
 * 
 * @code{.cpp}
 * class MyComponent : public RaeptorCogs::Component {
 *   // Custom component implementation
 * };
 * @endcode
 */
class Component : public RegisterNode<Component, Node> {
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

    public:

        // ============================================================================
        //                                PUBLIC METHODS
        // ============================================================================


        /**
         * @brief Default constructor for Component.
         */
        Component() = default;

        /**
         * @brief Virtual destructor for Component.
         * 
         * Ensures proper cleanup of derived classes.
         */
        virtual ~Component() = default;

        /**
         * @brief Set the renderer for this component.
         * 
         * @param renderer Pointer to the renderer.
         */
        void setRenderer(Singletons::Renderer* renderer);

        /**
         * @brief Get the renderer for this component.
         * 
         * @return Pointer to the renderer.
         */
        Singletons::Renderer* getRenderer() const;

        /**
         * @brief Update the component.
         * 
         * This method is called to update the component's state.
         */
        virtual void update(GAPI::Common::RenderPipeline& pipeline) = 0;

};


/**
 * @brief 2D Component class.
 * 
 * Provides an interface for 2D components that can be attached to nodes.
 * 
 * Typical use cases:
 * - Creating reusable 2D components for game objects or entities
 * 
 * @see Component
 */
class Component2D : public RegisterNode<Component2D, Component> {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

    public:

        // ============================================================================
        //                                PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Component2D.
         */
        Component2D() = default;

        /**
         * @brief Virtual destructor for Component2D.
         * 
         * Ensures proper cleanup of derived classes.
         */
        virtual ~Component2D() = default;

};

}