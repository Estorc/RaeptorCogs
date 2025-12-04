#include <RaeptorCogs/GAPI/GL/Core/Internal/ImGuiModule.hpp>
#ifdef USE_IMGUI
#include <RaeptorCogs/External/ImGui/ImGui.hpp>
#endif

namespace RaeptorCogs::GAPI::GL {

void ImGuiModule::createImGuiContext(ImGuiContext *&imGuiContext, GLFWwindow* window) {
    #ifdef USE_IMGUI
    IMGUI_CHECKVERSION();
    imGuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imGuiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_Init("#version 300 es");
        io.DisplaySize = ImVec2(frameWidth, frameHeight);
        io.DisplayFramebufferScale = ImVec2((float)dpr, (float)dpr);
    #else
        ImGui_ImplOpenGL3_Init("#version 460 core");
    #endif
    #endif
}

void ImGuiModule::NewImGuiFrame() {
    #ifdef USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    this->setPendingFrame(true);
    #endif
}

void ImGuiModule::RenderImGuiDrawData() {
    #ifdef USE_IMGUI
    if (!this->isPendingFrame()) {
        return;
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    this->setPendingFrame(false);
    #endif
}

}