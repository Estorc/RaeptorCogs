#ifdef __EMSCRIPTEN__
#include <RaeptorCogs/Core/Platform.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/GraphicCore.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/RenderPipeline.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/WebGL/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Graphic.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>


#include <GLFW/glfw3.h>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>


namespace RaeptorCogs::GAPI::WebGL {

void GraphicCore::initGraphics() {

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  Window *window = new Window(
      800,
      600,
      "",
      nullptr,
      nullptr,
      new WebGL::WindowContext(&this->getRenderer()),
      WindowFlags::CONTEXT_ONLY | WindowFlags::DESTROY_AFTER_INIT);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  window->makeContextCurrent();

#ifdef __EMSCRIPTEN__
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
#else
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
#endif
  std::cout << "WebGL initialized successfully." << std::endl;

  this->getRenderer().getPlatform().getWindows().push_back(window);
}

void GraphicCore::buildShaders() {
  std::vector<rgsl_shader_blob> shaders(
      rgsl_shaders, rgsl_shaders + sizeof(rgsl_shaders) / sizeof(rgsl_shader_blob));
  Common::ShaderInfo shaderInfo = {};
  shaderInfo.vertex_shader =
      FindEmbeddedShader(shaders, "shader_main", RGSL_VERTEX, "es");
  shaderInfo.fragment_shader =
      FindEmbeddedShader(shaders, "shader_main", RGSL_FRAGMENT, "es");
  this->getMainShader()->build(shaderInfo);
  shaderInfo.fragment_shader =
      FindEmbeddedShader(shaders, "shader_mask", RGSL_FRAGMENT, "es");
  this->getMaskShader()->build(shaderInfo);
}

void GraphicCore::drawElementsInstancedBaseVertexBaseInstance(
    size_t count, size_t instanceCount, size_t first, int baseVertex,
    unsigned int baseInstance) {
  (void)first;
  (void)baseVertex;

  this->getMainShader()->setInt("uBaseInstance", static_cast<int>(baseInstance));
  this->getMaskShader()->setInt("uBaseInstance", static_cast<int>(baseInstance));
  glDrawElementsInstanced(
      GL_TRIANGLES,
      static_cast<GLsizei>(count),
      GL_UNSIGNED_INT,
      (void *)(0 * sizeof(GLuint)),
      static_cast<GLsizei>(instanceCount));

  // glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES,
  // static_cast<GLsizei>(count), GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)),
  // static_cast<GLsizei>(instanceCount), baseVertex, baseInstance);
}

void GraphicCore::bindGraphicTexture(Graphic2D &graphic) {
  glActiveTexture(GL_TEXTURE0);
  graphic.bind();
}

void GraphicCore::bindMaskTexture() {
  glActiveTexture(GL_TEXTURE0 + this->getMaxTextureUnits() - 1);
  glBindTexture(GL_TEXTURE_2D, this->getMaskTextures().second->getID());
}

void GraphicCore::setTextureUniform(ObjectHandler<Common::Shader> shader) {
  shader->setInt("uTextureSampler", 0);
  shader->setInt("uIndirectionBuffer", 4);
  shader->setInt("uInstanceBuffer", 5);
  shader->setInt("uRawDataBuffer", 6);
}

void GraphicCore::setMaskTextureUniform(ObjectHandler<Common::Shader> shader) {
  shader->setInt("uMaskTextureSampler", static_cast<int>(this->getMaxTextureUnits() - 1));
}

} // namespace RaeptorCogs::GAPI::WebGL
#endif