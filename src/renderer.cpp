#include <RaeptorCogs/Renderer.hpp>
#include <RaeptorCogs/Graphic.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <iostream>
#include "shaders_embed.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif


namespace RaeptorCogs::Singletons {

constexpr int MAX_SPRITES = 1000000;
constexpr int INSTANCE_SIZE = 32;

#ifdef __EMSCRIPTEN__
constexpr int IDATATEX_HEIGHT = (MAX_SPRITES * INSTANCE_SIZE + 16 * IDATATEX_WIDTH - 1) / (16 * IDATATEX_WIDTH);
#else
constexpr int SSBO_DEFAULT_SIZE = (MAX_SPRITES * INSTANCE_SIZE);
#endif


#ifdef __EMSCRIPTEN__
EM_BOOL resize_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {

    RaeptorCogs::Window* window = static_cast<RaeptorCogs::Window*>(userData);

    int new_width = uiEvent->windowInnerWidth;
    int new_height = uiEvent->windowInnerHeight;

    printf("Window resized: %d x %d\n", new_width, new_height);

    int width = new_width;
    int height = new_height;
    #ifdef __EMSCRIPTEN__
    double dpr = emscripten_get_device_pixel_ratio();
    width = (int)round((double)width * dpr);
    height = (int)round((double)height * dpr);
    #endif
    window->setSize(width, height);
    #ifdef __EMSCRIPTEN__
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(new_width, new_height);
    io.DisplayFramebufferScale = ImVec2((float)dpr, (float)dpr);
    // set CSS size (in CSS pixels)
    emscripten_set_element_css_size("#canvas", new_width, new_height);
    // set backing size to CSS * DPR (in device pixels)
    emscripten_set_canvas_element_size("#canvas", width, height);
    // then call glViewport(0,0,backing_w,backing_h) in your GL code
    #endif

    return EM_TRUE; // event handled
}
#endif


Window* Renderer::initialize(int width, int height, const char* title) {

    #ifdef __EMSCRIPTEN__
    int frameWidth = GetScreenWidth();
    int frameHeight = GetScreenHeight();
    double dpr = emscripten_get_device_pixel_ratio();
    width = (int)round((double)GetScreenWidth() * dpr);
    height = (int)round((double)GetScreenHeight() * dpr);
    #endif

    this->windows.push_back(this->initGraphics(width, height, title));
    this->buildBuffers();
    this->buildVAO(this->windows.front());
    this->buildShader();

    #ifdef USE_IMGUI
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->windows.front()->getGLFWWindow(), true);
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_Init("#version 300 es");
    #else
        ImGui_ImplOpenGL3_Init("#version 460 core");
    #endif
    #endif

    #ifdef __EMSCRIPTEN__
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(frameWidth, frameHeight);
    io.DisplayFramebufferScale = ImVec2((float)dpr, (float)dpr);
    emscripten_set_element_css_size("#canvas", frameWidth, frameHeight);
    emscripten_set_canvas_element_size("#canvas", width, height);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this->windows.front(), EM_TRUE, resize_callback);
    #endif

    return this->windows.front();
}


Renderer::~Renderer() {
    #ifdef USE_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    #endif
    this->clearBatches();
    glDeleteBuffers(1, &this->quadVBO);
    glDeleteBuffers(1, &this->quadEBO);
    glDeleteBuffers(1, &this->instanceVBO);
    #ifndef __EMSCRIPTEN__
    glDeleteBuffers(1, &this->instanceSSBO);
    #else
    glDeleteTextures(1, &this->iDataTex);
    #endif
    glDeleteProgram(this->shader);
    for (Window* window : this->windows) {
        if (window) {
            delete window;
        }
    }
    glfwTerminate();
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


Window* Renderer::initGraphics(int width, int height, const char* title) {
    glfwSetErrorCallback(error_callback);


    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    #ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #endif

    Window* window = new Window(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window->getGLFWWindow());

    #ifndef __EMSCRIPTEN__
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    #else
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    #endif

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
    #ifndef __EMSCRIPTEN__
    glGenBuffers(1, &this->instanceSSBO);
    #else
    glGenTextures(1, &this->iDataTex);
    #endif

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
    glVertexAttribIPointer(7, 1, GL_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, type));
    glVertexAttribDivisor(7, 1);

    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(8, 1, GL_INT, sizeof(InstanceData), (void*)offsetof(InstanceData, dataOffset));
    glVertexAttribDivisor(8, 1);

    glBindVertexArray(0);

    // Shader Storage Buffer Object (SSBO) for instance data
    #ifndef __EMSCRIPTEN__
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->instanceSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, SSBO_DEFAULT_SIZE, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->instanceSSBO); // binding = 0
    #else
    glBindTexture(GL_TEXTURE_2D, this->iDataTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, IDATATEX_WIDTH, IDATATEX_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    #endif
}


void Renderer::buildShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    #ifdef __EMSCRIPTEN__
    glShaderSource(vertexShader, 1, &__shader__main_gles_vs, nullptr);
    #else
    glShaderSource(vertexShader, 1, &__shader__main_vs, nullptr);
    #endif
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    #ifdef __EMSCRIPTEN__
    glShaderSource(fragmentShader, 1, &__shader__main_gles_fs, nullptr);
    #else
    glShaderSource(fragmentShader, 1, &__shader__main_fs, nullptr);
    #endif
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
    windows.push_back(new Window(width, height, title, nullptr, this->windows.front()->getGLFWWindow()));
    if (!windows.back()) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    this->buildVAO(windows.back());

    return windows.back();
}

void Renderer::destroyWindow(Window* window) {
    if (window) {
        auto it = std::find(windows.begin(), windows.end(), window);
        if (it != windows.end()) {
            windows.erase(it);
        }
        delete window;
    }
}


/**
 * TODO: Make sorting smarter by forming smart groups of graphics
 * based on their properties (e.g., texture, z-index, etc.)
 * This will reduce the number of batches and improve performance.
 */
void Renderer::render(Window* window, int width, int height) {
    #ifdef USE_IMGUI
        ImGui::Render();
    #endif
    if (width == 0 || height == 0) {
        width = window->getWidth();
        height = window->getHeight();
    }
    glfwMakeContextCurrent(window->getGLFWWindow());

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Opaque objects

    glUseProgram(this->shader);
    glBindVertexArray(window->getQuadVAO());
    
    glUniform1f(glGetUniformLocation(this->shader, "Time"), static_cast<float>(glfwGetTime()));
    float nearPlane = -100.0f;
    float farPlane = 100.0f;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, nearPlane, farPlane);
    GLint projLoc = glGetUniformLocation(this->shader, "projection");
    glUniform1i(glGetUniformLocation(this->shader, "Texture"), 0);
    #ifdef __EMSCRIPTEN__
    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    glUniform1i(glGetUniformLocation(this->shader, "IDataTex"), maxTextureUnits - 1);
    #endif
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    
    for (auto& pair : this->batches) {
        this->instanceDataBuffer.clear(); // Clear the instance data buffer for each batch
        if (pair.second.empty()) continue; // Skip empty batches
        std::vector<InstanceData> instanceData;
        instanceData.resize(pair.second.size());
        for (size_t i = 0; i < pair.second.size(); ++i) {
            auto& graphic = pair.second[i];
            graphic->computeInstanceData(instanceData[i], this->instanceDataBuffer);
            if (!graphic->isVisible()) {
                instanceData[i].type = RENDERER_MODE_DEFAULT; // Default type
            }
        }

        if (pair.second[0]->isOpaque()) {
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND); // Disable blending for opaque objects
        } else {
            glDisable(GL_DEPTH_TEST); // Disable depth testing
            glEnable(GL_BLEND); // Enable blending for transparent objects
        }

        // Shared data for all instances
        glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(InstanceData), instanceData.data());

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        pair.second[0]->bind();

        // Per instance data
        #ifndef __EMSCRIPTEN__
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->instanceSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, this->instanceDataBuffer.size() * sizeof(float), this->instanceDataBuffer.data());
        #else
        glActiveTexture(GL_TEXTURE0 + maxTextureUnits - 1);
        glBindTexture(GL_TEXTURE_2D, this->iDataTex);
        size_t totalFloat = this->instanceDataBuffer.size(); // float count
        size_t texWidth = IDATATEX_WIDTH;
        size_t texHeight = ((totalFloat + 3) / 4 + texWidth - 1) / texWidth;
        this->instanceDataBuffer.resize(texWidth * texHeight * 4, 0.0f); // Resize to fit texture
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_FLOAT, this->instanceDataBuffer.data());
        #endif

        // Draw the quad
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceData.size());
    }
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    #if USE_IMGUI
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #endif
    glfwSwapBuffers(window->getGLFWWindow());
}



void Renderer::addGraphic(Graphic *graphic) {
    BatchKey key = std::make_tuple(graphic->getZIndex(), graphic->isOpaque(), graphic->getProgramID(), graphic->getID());
    graphic->setRenderer(this);
    graphic->setRendererKey(key);
    this->batches[key].push_back(graphic);
}

void Renderer::changeGraphicPosition(Graphic *graphic) {
    if (graphic->getRenderer() != this) {
        std::cerr << "Graphic not managed by this renderer!" << std::endl;
        return;
    }

    BatchKey newKey = std::make_tuple(graphic->getZIndex(), graphic->isOpaque(), graphic->getProgramID(), graphic->getID());
    BatchKey oldKey = graphic->getRendererKey();
    if (oldKey == newKey) return; // No change needed

    // Remove from old batch
    auto& oldBatch = this->batches[oldKey];
    auto it = std::remove(oldBatch.begin(), oldBatch.end(), graphic);
    if (it != oldBatch.end()) {
        oldBatch.erase(it, oldBatch.end());
    }

    // Add to new batch
    this->batches[newKey].push_back(graphic);
    graphic->setRendererKey(newKey);
}

void Renderer::removeGraphic(Graphic *graphic) {
    if (graphic->getRenderer() != this) {
        std::cerr << "Graphic not managed by this renderer!" << std::endl;
        return;
    }
    if (this->batches.empty()) {
        return;
    }

    BatchKey key = graphic->getRendererKey();
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

void Renderer::NewImGuiFrame() {
    #ifdef USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    #endif
}

};