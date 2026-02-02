/** ********************************************************************************
 * @section GAPI_Vulkan_WindowContext_Overview Overview
 * @file WindowContext.hpp
 * @brief Vulkan Window context implementation.
 * @details
 * Typical use cases:
 * - Defining Vulkan-specific window context implementations
 * *********************************************************************************
 * @section GAPI_Vulkan_WindowContext_Header Header
 * <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
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
#include <RaeptorCogs/Graphics/GAPI/Common/Core/Internal/WindowContext.hpp>
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
         * @brief Surface capabilities.
         * 
         * Holds the capabilities of the Vulkan surface.
         */
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        /**
         * @brief Surface format.
         * 
         * Holds the format of the Vulkan surface.
         */
        VkSurfaceFormatKHR surfaceFormat;

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
         * @brief Swapchain images.
         * 
         * Holds the images used in the swapchain.
         */
        std::vector<VkImage> swapchainImages;

        /**
         * @brief Swapchain image views.
         * 
         * Holds the image views for the swapchain images.
         */
        std::vector<VkImageView> swapchainImageViews;

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
         * @brief Descriptor pools.
         * 
         * Holds Vulkan descriptor pools for resource management.
         */
        std::vector<VkDescriptorPool> descriptorPools;

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

    protected:

        // ============================================================================
        //                             PROTECTED METHODS
        // ============================================================================

        /**
         * @brief Create Vulkan surface and render pass.
         */
        void createSurfaceAndRenderPass();

        /**
         * @brief Create swapchain images.
         */
        void createSwapchainImages();

        /**
         * @brief Create image views for swapchain images.
         */
        void createImageViews();

        /**
         * @brief Create framebuffers for swapchain images.
         */
        void createFramebuffers();

        /**
         * @brief Create command buffers for rendering.
         */
        void createCommandBuffers();

        /**
         * @brief Create synchronization objects (semaphores and fences).
         */
        void createFencesAndSemaphores();

        /**
         * @brief Create descriptor pools.
         */
        void createDescriptorPools();
        
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
         * @brief Get the associated window.
         * 
         * @return Pointer to the associated Window instance.
         */
        Window* getWindow() const { return window; };
        
        /**
         * @brief Get the swapchain extent.
         * 
         * @return Reference to the VkExtent2D representing the swapchain extent.
         * 
         * @note Contains width and height of the swapchain images.
         */
        VkExtent2D& getSwapExtent() { return swapExtent; }

        /**
         * @brief Get the Vulkan surface.
         * 
         * @return Reference to the VkSurfaceKHR representing the Vulkan surface.
         */
        VkSwapchainKHR& getSwapchain() { return swapchain; }

        /**
         * @brief Get the Vulkan surface.
         * 
         * @return Reference to the VkSurfaceKHR representing the Vulkan surface.
         */
        VkSurfaceKHR& getSurface() { return surface; }

        /**
         * @brief Get the surface capabilities.
         * 
         * @return Reference to the VkSurfaceCapabilitiesKHR representing the surface capabilities.
         */
        VkSurfaceCapabilitiesKHR& getSurfaceCapabilities() { return surfaceCapabilities; }

        /**
         * @brief Get the surface format.
         * 
         * @return Reference to the VkSurfaceFormatKHR representing the surface format.
         */
        VkSurfaceFormatKHR& getSurfaceFormat() { return surfaceFormat; }

        /**
         * @brief Get the depth image.
         * 
         * @return Reference to the VkImage representing the depth image.
         */
        VkImage& getDepthImage() { return depthImage; }

        /**
         * @brief Get the depth image memory.
         * 
         * @return Reference to the VkDeviceMemory representing the depth image memory.
         */
        VkDeviceMemory& getDepthImageMemory() { return depthImageMemory; }

        /**
         * @brief Get the depth image view.
         * 
         * @return Reference to the VkImageView representing the depth image view.
         */
        VkImageView& getDepthImageView() { return depthImageView; }

        /**
         * @brief Get the swapchain images.
         * 
         * @return Reference to the vector of VkImage representing the swapchain images.
         */
        std::vector<VkImage>& getSwapchainImages() { return swapchainImages; }

        /**
         * @brief Get the swapchain image views.
         * 
         * @return Reference to the vector of VkImageView representing the swapchain image views.
         */
        std::vector<VkImageView>& getSwapchainImageViews() { return swapchainImageViews; }

        /**
         * @brief Get the framebuffers.
         * 
         * @return Reference to the vector of VkFramebuffer representing the framebuffers.
         */
        std::vector<VkFramebuffer>& getFramebuffers() { return framebuffers; }

        /**
         * @brief Get the command buffers.
         * 
         * @return Reference to the vector of VkCommandBuffer representing the command buffers.
         */
        std::vector<VkCommandBuffer>& getCommandBuffers() { return commandBuffers; }

        /**
         * @brief Get synchronization objects.
         * 
         * @return References to the vectors of semaphores and fences used for synchronization.
         */
        std::vector<VkSemaphore>& getImageAvailableSemaphores() { return imageAvailableSemaphores; }

        /**
         * @brief Get render finished semaphores.
         * 
         * @return References to the vector of VkSemaphore used for render finished synchronization.
         */
        std::vector<VkSemaphore>& getRenderFinishedSemaphores() { return renderFinishedSemaphores; }

        /**
         * @brief Get in-flight fences.
         * 
         * @return References to the vector of VkFence used for in-flight frame synchronization.
         */
        std::vector<VkFence>& getInFlightFences() { return inFlightFences; }

        /**
         * @brief Get images in-flight fences.
         * 
         * @return References to the vector of VkFence used for tracking images in flight.
         */
        std::vector<VkFence>& getImagesInFlight() { return imagesInFlight; }

        /**
         * @brief Get the maximum number of frames in flight.
         * 
         * @return Constant reference to the maximum number of frames in flight.
         */
        size_t getMaxFramesInFlight() const { return MAX_FRAMES_IN_FLIGHT; }

        /**
         * @brief Get descriptor pools.
         * 
         * @return References to the vector of VkDescriptorPool used for resource management.
         */
        std::vector<VkDescriptorPool>& getDescriptorPools() { return descriptorPools; }

        /**
         * @brief Get synchronization objects for the current frame.
         * 
         * @param index Index of the frame.
         * @return References to the semaphores and fences for the specified frame.
         */
        VkSemaphore& getImageAvailableSemaphore(size_t index) { return imageAvailableSemaphores[index]; }

        /**
         * @brief Get render finished semaphore for the current frame.
         * 
         * @param index Index of the frame.
         * @return Render finished semaphore for the specified frame.
         */
        VkSemaphore& getRenderFinishedSemaphore(size_t index) { return renderFinishedSemaphores[index]; }

        /**
         * @brief Get in-flight fence for the current frame.
         * 
         * @param index Index of the frame.
         * @return In-flight fence for the specified frame.
         */
        VkFence& getInFlightFence(size_t index) { return inFlightFences[index]; }

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
        size_t& getCurrentFrame() { return currentFrame; }

        /**
         * @brief Advance to the next frame.
         * 
         * Updates the current frame index in a circular manner.
         */
        void advanceFrame() { currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; }
};

}