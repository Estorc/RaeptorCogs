#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/ImGuiModule.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <iostream>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif
namespace RaeptorCogs::GAPI::Vulkan {

void ImGuiModule::createImGuiContext(ImGuiContext *&imGuiContext, Window* window) {
    IMGUI_CHECKVERSION();
    imGuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imGuiContext);
    ImGui_ImplGlfw_InitForVulkan(window->getGLFWWindow(), true);
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

    //init_info.UseDynamicRendering = true;
    //init_info.ColorAttachmentFormat = static_cast<WindowContext*>(window->getContext())->getSurfaceFormat().format;
    
    ImGui_ImplVulkan_Init(&init_info, static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getRenderPass());
    // Upload Fonts
    VkCommandBuffer cmd = BeginSingleTimeCommands(); // record buffer

    ImGui_ImplVulkan_CreateFontsTexture(cmd);

    EndSingleTimeCommands(cmd); // submit and wait

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiModule::NewImGuiFrame() {
    #ifdef USE_IMGUI
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    #endif
}

/** @warning :UNUSED: */
void ImGuiModule::RenderImGuiDrawData() {
    #ifdef USE_IMGUI
    //ImGui::Render();
    //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<RaeptorCogs::GAPI::Vulkan::RendererBackend*>(&RaeptorCogs::Renderer().getBackend())->getGraphicCore().getCommandBuffer());
    #endif

}

}
#endif // VULKAN_SUPPORTED