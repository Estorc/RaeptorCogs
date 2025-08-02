#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Window {
    private:
        GLFWwindow* window;
        std::string title;
        GLuint quadVAO;

    public:
        Window(int width, int height, const std::string& title, GLFWmonitor* monitor, GLFWwindow* share);
        ~Window();

        GLFWwindow* getGLFWWindow() const;
        int getWidth() const;
        int getHeight() const;
        glm::ivec2 getSize() const;
        const std::string& getTitle() const;
        GLuint& getQuadVAO();

        void setTitle(const std::string& newTitle);
};