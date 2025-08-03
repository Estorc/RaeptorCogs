#include <RaeptorLab/renderer.hpp>
#include <iostream>
#include "shaders_embed.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#define MAX_SPRITES 1000000
#define SSBO_INSTANCE_SIZE 32
#define SSBO_DEFAULT_SIZE (MAX_SPRITES * SSBO_INSTANCE_SIZE)


Window* Renderer::initialize(int width, int height, const char* title) {
    this->mainWindow = this->initGraphics(width, height, title);
    this->buildBuffers();
    this->buildVAO(this->mainWindow);
    this->buildShader();
    return this->mainWindow;
}


void Renderer::destroy() {
    glDeleteBuffers(1, &this->quadVBO);
    glDeleteBuffers(1, &this->quadEBO);
    glDeleteBuffers(1, &this->instanceVBO);
    glDeleteBuffers(1, &this->instanceSSBO);
    glDeleteProgram(this->shader);
}

Window* Renderer::initGraphics(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    Window* window = new Window(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window->getGLFWWindow());

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    return window;
} 

void Renderer::buildBuffers() {
    float vertices[] = {
        0.0f,  0.0f,      0.0f,   0.0f,
         1.0f,  0.0f,      1.0f,   0.0f,
         1.0f,   1.0f,      1.0f,   1.0f,
        0.0f,   1.0f,      0.0f,   1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,   // First triangle
        2, 3, 0    // Second triangle
    };

    glGenBuffers(1, &this->quadVBO);
    glGenBuffers(1, &this->instanceVBO);
    glGenBuffers(1, &this->quadEBO);
    glGenBuffers(1, &this->instanceSSBO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_SPRITES * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);
}

void Renderer::buildVAO(Window* window) {
    //glfwMakeContextCurrent(window->getGLFWWindow());

    glGenVertexArrays(1, &window->getQuadVAO());
    glBindVertexArray(window->getQuadVAO());
    glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->quadEBO);

    // Vertex attributes
    // Position (0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // TexCoord (1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Instance matrix (location 2-5)
    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);

    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, model) + i * sizeof(glm::vec4)));
        glVertexAttribDivisor(2 + i, 1);
    }

    // UV Rect attribute (location 6)
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, uvRect));
    glVertexAttribDivisor(6, 1);

    // Setup VBO for InstanceHeader:
    glEnableVertexAttribArray(7);
    glVertexAttribIPointer(7, 1, GL_UNSIGNED_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, type));
    glVertexAttribDivisor(7, 1);

    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(8, 1, GL_UNSIGNED_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, dataOffset));
    glVertexAttribDivisor(8, 1);

    glBindVertexArray(0);

    // Shader Storage Buffer Object (SSBO) for instance data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->instanceSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, SSBO_DEFAULT_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->instanceSSBO); // binding = 0
}


void Renderer::buildShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &__shader__main_vs, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &__shader__main_fs, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    this->shader = glCreateProgram();
    glAttachShader(this->shader, vertexShader);
    glAttachShader(this->shader, fragmentShader);
    glLinkProgram(this->shader);
    glGetProgramiv(this->shader, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


Window* Renderer::createWindow(int width, int height, const char* title) {
    Window* window = new Window(width, height, title, nullptr, this->mainWindow->getGLFWWindow());
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    this->buildVAO(window);

    return window;
}

void Renderer::destroyWindow(Window* window) {
    if (window) {
        delete window;
    }
}


void Renderer::render(Window* window) {
    int width = window->getWidth();
    int height = window->getHeight();
    glfwMakeContextCurrent(window->getGLFWWindow());

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(this->shader);
    glBindVertexArray(window->getQuadVAO());
    
    glUniform1f(glGetUniformLocation(this->shader, "Time"), static_cast<float>(glfwGetTime()));
    float nearPlane = -100.0f;
    float farPlane = 100.0f;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, nearPlane, farPlane);
    GLint projLoc = glGetUniformLocation(this->shader, "projection");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(this->shader, "Texture"), 0);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    for (auto& pair : this->batches) {
        this->instanceDataBuffer.clear(); // Clear the instance data buffer for each batch
        if (pair.second.empty()) continue; // Skip empty batches
        std::vector<InstanceData> instanceData;
        instanceData.resize(pair.second.size());
        for (size_t i = 0; i < pair.second.size(); ++i) {
            auto& graphic = pair.second[i];
            graphic->computeInstanceData(instanceData[i], this->instanceDataBuffer);
        }

        // Shared data for all instances
        glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(InstanceData), instanceData.data());

        // Per instance data
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->instanceSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, this->instanceDataBuffer.size(), this->instanceDataBuffer.data());
        
        // Bind texture
        pair.second[0]->bind();

        // Draw the quad
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceData.size());
    }
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}



void Renderer::addGraphic(Graphic *graphic) {
    GLuint textureID = graphic->getID();
    graphic->setRenderer(this);
    graphic->setRendererKey(textureID);
    this->batches[textureID].push_back(graphic);
}

void Renderer::changeGraphicPosition(Graphic *graphic, GLuint newTextureID) {
    if (graphic->getRenderer() != this) {
        std::cerr << "Graphic not managed by this renderer!" << std::endl;
        return;
    }

    GLuint oldTextureID = graphic->getRendererKey();
    if (oldTextureID == newTextureID) return; // No change needed

    // Remove from old batch
    auto& oldBatch = this->batches[oldTextureID];
    auto it = std::remove(oldBatch.begin(), oldBatch.end(), graphic);
    if (it != oldBatch.end()) {
        oldBatch.erase(it, oldBatch.end());
    }

    // Add to new batch
    this->batches[newTextureID].push_back(graphic);
    graphic->setRendererKey(newTextureID);
}

void Renderer::removeGraphic(Graphic *graphic) {
    if (graphic->getRenderer() != this) {
        std::cerr << "Graphic not managed by this renderer!" << std::endl;
        return;
    }
    if (this->batches.empty()) {
        return;
    }

    GLuint key = graphic->getRendererKey();
    auto& batch = this->batches[key];
    auto it = std::remove(batch.begin(), batch.end(), graphic);
    if (it != batch.end()) {
        batch.erase(it, batch.end());
    }
    
    graphic->setRenderer(nullptr); // Clear the renderer reference
}

void Renderer::clearBatches() {
    this->batches.clear();
}