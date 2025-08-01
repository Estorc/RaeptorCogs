#pragma once
#include <main.hpp>
#include <renderer.hpp>

class RaeptorApplication {
    private:
        GLFWwindow* window;
        Renderer* renderer;

    public:
        RaeptorApplication();
        ~RaeptorApplication();

        int update();

        ImGuiIO& getGui();
        GLFWwindow* getWindow();
};