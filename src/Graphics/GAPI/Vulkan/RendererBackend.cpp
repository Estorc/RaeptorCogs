#ifdef VULKAN_SUPPORTED
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Internal/WindowContext.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>

namespace RaeptorCogs::GAPI::Vulkan {

VkDebugUtilsMessengerEXT debugMessenger;

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  (void)messageType;
  (void)pUserData;
  std::cerr << "[VULKAN] " << pCallbackData->pMessage << std::endl;
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    throw std::runtime_error("Vulkan validation layer reported an error.");
  }
  return VK_FALSE;
}

void RendererBackend::createInstance() {
  std::vector<const char *> extensions;
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  if (!glfwVulkanSupported()) {
    if (glfwGetError(NULL) == GLFW_NOT_INITIALIZED) {
      std::cerr << "GLFW is not initialized!" << std::endl;
    } else {
      std::cerr << "Vulkan loader not found!" << std::endl;
    }
    std::cerr << "GLFW Vulkan support is not available" << std::endl;
    return;
  }
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  if (!glfwExtensions) {
    std::cerr << "Failed to get required GLFW Vulkan extensions" << std::endl;
    return;
  }

  extensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
  // Add the debug utils extension
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  // Optional: Enable validation layers
  std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };

  VkApplicationInfo appInfo {};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = "RaeptorCogs App";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName        = "RaeptorCogs";
  appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion         = VK_API_VERSION_1_3;

  VkInstanceCreateInfo createInfo {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo        = &appInfo;
  createInfo.enabledExtensionCount   = (uint32_t)extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (checkValidationLayerSupport(validationLayers)) {
    createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    std::cerr << "Validation layers requested, but not available!" << std::endl;
  }

  vkCreateInstance(&createInfo, NULL, &this->getGraphicCore().getInstance());
}

void RendererBackend::setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
  debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugCreateInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debugCreateInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debugCreateInfo.pfnUserCallback = debugCallback;

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      this->getGraphicCore().getInstance(), "vkCreateDebugUtilsMessengerEXT");
  if (func) {
    std::cout << "Setting up Vulkan debug messenger." << std::endl;
    func(
        this->getGraphicCore().getInstance(), &debugCreateInfo, nullptr, &debugMessenger);
  } else {
    std::cerr << "Could not set up debug messenger!" << std::endl;
  }
}

void RendererBackend::pickPhysicalDevice() {
  this->getGraphicCore().getPhysicalDevice() = VK_NULL_HANDLE;
  uint32_t physicalDeviceCount               = 0;
  vkEnumeratePhysicalDevices(
      this->getGraphicCore().getInstance(), &physicalDeviceCount, NULL);
  if (physicalDeviceCount == 0) {
    std::cerr << "Failed to find GPUs with Vulkan support!" << std::endl;
    return;
  }
  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(
      this->getGraphicCore().getInstance(), &physicalDeviceCount, physicalDevices.data());
  // Find the best suitable physical device
  int bestScore = 0;
  for (const auto &device : physicalDevices) {
    int score = rateDeviceSuitability(device);
    if (score >= bestScore) {
      bestScore                                  = score;
      this->getGraphicCore().getPhysicalDevice() = device;
    }
  }
  if (this->getGraphicCore().getPhysicalDevice() == VK_NULL_HANDLE) {
    std::cerr << "Failed to find a suitable GPU!" << std::endl;
    return;
  } else {
    std::cout << "Selected GPU: ";
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(
        this->getGraphicCore().getPhysicalDevice(), &deviceProperties);
    std::cout << deviceProperties.deviceName << std::endl;
  }
}

void RendererBackend::createQueueAndLogicalDevice() {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      this->getGraphicCore().getPhysicalDevice(), &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      this->getGraphicCore().getPhysicalDevice(),
      &queueFamilyCount,
      queueFamilies.data());

  /**
   * TODO: Ensure that the selected queue family supports both graphics and presentation.
   * For simplicity, we assume that graphics queue also supports presentation.
   */

  std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      float queuePriority                                  = 1.0f;
      this->getGraphicCore().getGraphicsQueueFamilyIndex() = i;
      VkDeviceQueueCreateInfo queueCreateInfo {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex =
          this->getGraphicCore().getGraphicsQueueFamilyIndex();
      queueCreateInfo.queueCount       = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;

      VkPhysicalDeviceDynamicRenderingFeatures dynFeat = {};
      dynFeat.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
      dynFeat.dynamicRendering = VK_TRUE;

      VkDeviceCreateInfo deviceCreateInfo {};
      deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      deviceCreateInfo.pNext                = &dynFeat;
      deviceCreateInfo.queueCreateInfoCount = 1;
      deviceCreateInfo.pQueueCreateInfos    = &queueCreateInfo;
      deviceCreateInfo.enabledExtensionCount =
          static_cast<uint32_t>(deviceExtensions.size());
      deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

      vkCreateDevice(
          this->getGraphicCore().getPhysicalDevice(),
          &deviceCreateInfo,
          nullptr,
          &this->getGraphicCore().getDevice());
      vkGetDeviceQueue(
          this->getGraphicCore().getDevice(),
          i,
          0,
          &this->getGraphicCore().getGraphicsQueue());
      vkGetDeviceQueue(
          this->getGraphicCore().getDevice(),
          i,
          0,
          &this->getGraphicCore().getPresentQueue());
      break;
    }
  }
}

void RendererBackend::createPipelineCache() {
  VkPipelineCacheCreateInfo cacheInfo {};
  cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

  vkCreatePipelineCache(
      this->getGraphicCore().getDevice(),
      &cacheInfo,
      nullptr,
      &this->getGraphicCore().getPipelineCache());
}

void RendererBackend::createCommandPool() {
  VkCommandPoolCreateInfo poolInfo {};
  poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = this->getGraphicCore().getGraphicsQueueFamilyIndex();
  poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  // allows resetting command buffers individually

  if (vkCreateCommandPool(
          this->getGraphicCore().getDevice(),
          &poolInfo,
          nullptr,
          &this->getGraphicCore().getCommandPool()) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void RendererBackend::createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 11> pool_sizes = {
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
  };

  VkDescriptorPoolCreateInfo pool_info {};
  pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets       = 1000 * static_cast<uint32_t>(pool_sizes.size());
  pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_info.pPoolSizes    = pool_sizes.data();

  vkCreateDescriptorPool(
      this->getGraphicCore().getDevice(),
      &pool_info,
      nullptr,
      &this->getImGuiModule().getImGuiDescriptorPool());
}

void RendererBackend::initialize() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  this->createInstance();
  this->setupDebugMessenger();
  this->pickPhysicalDevice();
  this->createQueueAndLogicalDevice();
  this->createPipelineCache();
  this->createCommandPool();
  this->createDescriptorPool(); // For ImGui
  // Create Surface, Swap Chain, Image Views during set up of window context

  std::cout << "Vulkan Renderer Backend initialized." << std::endl;
}

void RendererBackend::render(Window *window, int x, int y, int width, int height) {

  this->getGraphicCore().setRenderTarget(window, x, y, width, height);
  this->getRenderPipeline().renderMask(window, x, y, width, height);
  this->getGraphicCore().setRenderTarget(window, x, y, width, height);
  this->getRenderPipeline().renderPass(x, y, width, height);
}

void RendererBackend::render(Texture &texture, int x, int y, int width, int height) {
  (void)texture;
  (void)x;
  (void)y;
  (void)width;
  (void)height;
  // Vulkan-specific rendering code for texture would go here
}

Common::WindowContext *RendererBackend::createWindowContext() {
  return new WindowContext(this);
}

bool RendererBackend::isInitialized() const {
  // Vulkan-specific code to check initialization status would go here
  return this->initialized;
}

void RendererBackend::initializePipeline() {
  if (this->initialized) {
    return;
  }
  this->initialized = true;

  Common::ShaderInfo shaderInfo = {};
  shaderInfo.vertex_shader =
      FindEmbeddedShader("shader_example", Common::ShaderStage::VERTEX, "core");
  shaderInfo.fragment_shader =
      FindEmbeddedShader("shader_example", Common::ShaderStage::FRAGMENT, "core");
  this->getGraphicCore().getMainShader()->build(shaderInfo);

  std::vector<RaeptorCogs::Vertex2D> quadVertices = {
    { { 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 1.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { { 0.0f, 1.0f }, { 0.0f, 1.0f } }
  };
  uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

  this->getGraphicCore().getQuadVBO()->build(
      quadVertices.data(), sizeof(RaeptorCogs::Vertex2D) * quadVertices.size());
  this->getGraphicCore().getQuadEBO()->build(indices, sizeof(indices));
}

RendererBackend::~RendererBackend() {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      this->getGraphicCore().getInstance(), "vkDestroyDebugUtilsMessengerEXT");
  if (func) {
    func(this->getGraphicCore().getInstance(), debugMessenger, nullptr);
  }

  // Window contexts, swap chains, image views should be destroyed before this point
  for (Window *window : RaeptorCogs::Platform().getWindows()) {
    delete window;
  }
  RaeptorCogs::Platform().getWindows().clear();

  // vkFreeMemory(this->getGraphicCore().getDevice(),
  // this->getGraphicCore().getStagingBufferMemory(), nullptr);

  vkDestroyCommandPool(
      this->getGraphicCore().getDevice(),
      this->getGraphicCore().getCommandPool(),
      nullptr);
  this->getObjectAllocator().destroyAll();
  vkDestroyPipelineCache(
      this->getGraphicCore().getDevice(),
      this->getGraphicCore().getPipelineCache(),
      nullptr);
  this->getImGuiModule().destroyImGuiContext();
  vkDestroyDevice(this->getGraphicCore().getDevice(), nullptr);
  vkDestroyInstance(this->getGraphicCore().getInstance(), nullptr);

  std::cout << "Vulkan Renderer Backend destroyed." << std::endl;
}

}; // namespace RaeptorCogs::GAPI::Vulkan
#endif // VULKAN_SUPPORTED