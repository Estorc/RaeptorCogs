/** ********************************************************************************
 * @section GAPI_Vulkan_WindowContext_Overview Overview
 * @file WindowContext.hpp
 * @brief Vulkan Window context implementation.
 * @details
 * Typical use cases:
 * - Defining Vulkan-specific window context implementations
 * *********************************************************************************
 * @section GAPI_Vulkan_WindowContext_Header Header
 * <RaeptorCogs/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
 ***********************************************************************************
 * @section GAPI_Vulkan_WindowContext_Metadata Metadata
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
#include <RaeptorCogs/GAPI/Common/Core/Internal/WindowContext.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace RaeptorCogs::GAPI::Vulkan {

/**
 * @brief Vulkan Window context implementation.
 * 
 * Provides Vulkan-specific implementations for the window context interface.
 */
class WindowContext : public Common::WindowContext {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the associated window.
         * 
         * Holds a pointer to the Window instance this context is associated with.
         */
        Window* window;

        // --------------------------------------------
        //             Vulkan core objects
        // --------------------------------------------

        /**
         * @brief Swapchain extent (width and height).
         * 
         * Holds the dimensions of the swapchain images.
         */
        VkExtent2D swapExtent;

        /**
         * @brief Vulkan surface.
         * 
         * Holds the Vulkan surface associated with the window.
         */
        VkSurfaceKHR surface;

        /**
         * @brief Vulkan swapchain.
         * 
         * Holds the Vulkan swapchain used for presenting images to the window.
         */
        VkSwapchainKHR swapchain;

        /**
         * @brief Depth image resources.
         * 
         * Holds the depth image, its memory, and image view.
         */
        VkImage depthImage;

        /**
         * @brief Depth image memory.
         * 
         * Holds the memory allocated for the depth image.
         */
        VkDeviceMemory depthImageMemory;

        /**
         * @brief Depth image view.
         * 
         * Holds the image view for the depth image.
         */
        VkImageView depthImageView;

        /**
         * @brief Framebuffers for the swapchain images.
         * 
         * Holds the framebuffers used for rendering to the swapchain images.
         */
        std::vector<VkFramebuffer> framebuffers;

        /**
         * @brief Command buffers for rendering.
         * 
         * Holds the command buffers used for rendering operations.
         */
        std::vector<VkCommandBuffer> commandBuffers;


        /**
         * @brief Maximum number of frames that can be processed concurrently.
         * 
         * Holds the maximum number of frames in flight.
         */
        const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

        /**
         * @brief Synchronization objects for image availability.
         * 
         * Holds semaphores for signaling when an image is available for rendering.
         */
        std::vector<VkSemaphore> imageAvailableSemaphores;

        /**
         * @brief Synchronization objects for render completion.
         * 
         * Holds semaphores for signaling when rendering is finished.
         */
        std::vector<VkSemaphore> renderFinishedSemaphores;

        /**
         * @brief Fences for frame synchronization.
         * 
         * Holds fences to ensure that frames are rendered in order.
         */
        std::vector<VkFence> inFlightFences;

        /**
         * @brief Fences for tracking images in flight.
         * 
         * Holds fences to track which images are currently being rendered to.
         */
        std::vector<VkFence> imagesInFlight;

        /**
         * @brief Current frame index.
         * 
         * Holds the index of the current frame being processed.
         */
        size_t currentFrame = 0;

        // ============================================================================
        //                               PRIVATE METHODS
        // ============================================================================

        /**
         * @brief Destroy swapchain and related resources.
         * 
         * Cleans up Vulkan swapchain, framebuffers, and related resources.
         */
        void destroySwapchainResources();

    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Vulkan WindowContext.
         */
        WindowContext(Common::RendererBackend* renderer) : Common::WindowContext(renderer) {}

        /**
         * @brief Destructor for Vulkan WindowContext.
         */
        ~WindowContext() override;

        /**
         * @see RaeptorCogs::GAPI::Common::WindowContext::initialize()
         */
        void initialize(Window* window) override;

        /**
         * @brief Bind the window context for rendering.
         */
        void bind() override;

        // --------------------------------------------
        //          Vulkan specific methods
        // --------------------------------------------

        /**
         * @brief Recreate the swapchain and related resources.
         * 
         * @param window Pointer to the window (optional).
         * 
         * @note Called when the window is resized or the swapchain becomes invalid.
         */
        void recreateSwapchain(Window* window = nullptr);
        
        /**
         * @brief Get the swapchain extent.
         * 
         * @return Reference to the VkExtent2D representing the swapchain extent.
         * 
         * @note Contains width and height of the swapchain images.
         */
        const VkExtent2D& getSwapExtent() const { return swapExtent; }

        /**
         * @brief Get the Vulkan surface.
         * 
         * @return Reference to the VkSurfaceKHR representing the Vulkan surface.
         */
        const VkSwapchainKHR& getSwapchain() const { return swapchain; }

        /**
         * @brief Get the depth image.
         * 
         * @return Reference to the VkImage representing the depth image.
         */
        const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

        /**
         * @brief Get the command buffers.
         * 
         * @return Reference to the vector of VkCommandBuffer representing the command buffers.
         */
        const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }

        /**
         * @brief Get synchronization objects for the current frame.
         * 
         * @param index Index of the frame.
         * @return References to the semaphores and fences for the specified frame.
         */
        VkSemaphore getImageAvailableSemaphore(size_t index) const { return imageAvailableSemaphores[index]; }

        /**
         * @brief Get render finished semaphore for the current frame.
         * 
         * @param index Index of the frame.
         * @return Render finished semaphore for the specified frame.
         */
        VkSemaphore getRenderFinishedSemaphore(size_t index) const { return renderFinishedSemaphores[index]; }

        /**
         * @brief Get in-flight fence for the current frame.
         * 
         * @param index Index of the frame.
         * @return In-flight fence for the specified frame.
         */
        const VkFence& getInFlightFence(size_t index) const { return inFlightFences[index]; }

        /**
         * @brief Get image in-flight fence for the specified image index.
         * 
         * @param index Index of the swapchain image.
         * @return Image in-flight fence for the specified image.
         */
        VkFence& getImageInFlight(size_t index) { return imagesInFlight[index]; }

        /**
         * @brief Get the current frame index.
         * 
         * @return Current frame index.
         */
        const size_t& getCurrentFrame() const { return currentFrame; }

        /**
         * @brief Advance to the next frame.
         * 
         * Updates the current frame index in a circular manner.
         */
        void advanceFrame() { currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; }
};

}