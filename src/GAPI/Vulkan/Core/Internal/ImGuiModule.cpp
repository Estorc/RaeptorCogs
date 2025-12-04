#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/GAPI/Vulkan/Core/Internal/ImGuiModule.hpp>
#include <iostream>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif
namespace RaeptorCogs::GAPI::Vulkan {

VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getDevice(), &allocInfo, &commandBuffer);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getGraphicsQueue());

    vkFreeCommandBuffers(static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getDevice(), static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getCommandPool(), 1, &commandBuffer);
}

void ImGuiModule::createImGuiContext(ImGuiContext *&imGuiContext, GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    imGuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imGuiContext);
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplVulkan_InitInfo init_info = {}; 
    init_info.Instance = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getInstance();
    init_info.PhysicalDevice = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getPhysicalDevice();
    init_info.Device = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getDevice();
    init_info.Queue = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getGraphicsQueue();
    init_info.PipelineCache = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getPipelineCache();
    init_info.DescriptorPool = static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getImGuiModule().getImGuiDescriptorPool();
    init_info.MinImageCount = 2; // Arbitrary, but should be at least 2
    init_info.ImageCount = 2; // Arbitrary, should match swapchain image count

    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr; // custom allocator optional
    
    ImGui_ImplVulkan_Init(&init_info, static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getRenderPass());
    // Upload Fonts
    VkCommandBuffer cmd = beginSingleTimeCommands(); // record buffer

    ImGui_ImplVulkan_CreateFontsTexture(cmd);

    endSingleTimeCommands(cmd); // submit and wait

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiModule::NewImGuiFrame() {
    #ifdef USE_IMGUI
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    #endif
}

void ImGuiModule::RenderImGuiDrawData() {
    #ifdef USE_IMGUI
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getCommandBuffer());
    #endif

}

}