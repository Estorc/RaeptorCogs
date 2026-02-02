#ifdef VULKAN_SUPPORTED
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Resources/Shader.hpp>
#include <RaeptorCogs/EmbedSPIRVShaders.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/Core/Vulkan.hpp>
#include <RaeptorCogs/Graphics/GAPI/Vulkan/RendererBackend.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <RaeptorCogs/Graphics/Vertex.hpp>
#include <iostream>

namespace RaeptorCogs::GAPI::Vulkan {

VkShaderModule CreateShaderModule(VkDevice device, const uint32_t* code, size_t size) {

    VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = size * sizeof(uint32_t);
        info.pCode = code;

    VkShaderModule module;
    vkCreateShaderModule(device, &info, NULL, &module);

    return module;
}


/*Shader::Shader(const char* vertexSource, const char* fragmentSource) {
    this->vertexSource = vertexSource;
    this->fragmentSource = fragmentSource;
}*/

Shader::Shader() {

}

Shader::~Shader() {
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    VkDevice device = vkRenderer.getGraphicCore().getDevice();

    if (this->pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, this->pipeline, nullptr);
        this->pipeline = VK_NULL_HANDLE;
    }
}

void Shader::build(const Common::ShaderInfo &shaderInfo) {
    if (!shaderInfo.vertex_shader || !shaderInfo.fragment_shader) {
        std::cerr << "ERROR::VULKAN_SHADER::BUILD::SOURCE_CODE_NULL" << std::endl;
        return;
    }
    RendererBackend& vkRenderer = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend());
    VkDevice device = vkRenderer.getGraphicCore().getDevice();
    WindowContext &ctx = *static_cast<WindowContext*>(RaeptorCogs::Platform().getWindows().front()->getContext());
    rgsl_shader_blob* vertex_blob = (rgsl_shader_blob*)shaderInfo.vertex_shader;
    rgsl_shader_blob* fragment_blob = (rgsl_shader_blob*)shaderInfo.fragment_shader;
    VkShaderModule vertModule = CreateShaderModule(device, vertex_blob->spirv_words, vertex_blob->word_count);
    VkShaderModule fragModule = CreateShaderModule(device, fragment_blob->spirv_words, fragment_blob->word_count);

    // Store shader modules for later use in pipeline creation
    VkPipelineShaderStageCreateInfo vertStage = {};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = vertModule;
        vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage = {};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = fragModule;
        fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    std::vector<VkVertexInputAttributeDescription> attrDescs(2);
        attrDescs[0].location = 0;
        attrDescs[0].binding  = 0;
        attrDescs[0].format   = VK_FORMAT_R32G32_SFLOAT;
        attrDescs[0].offset   = offsetof(Vertex2D, position);

        attrDescs[1].location = 1;
        attrDescs[1].binding  = 0;
        attrDescs[1].format   = VK_FORMAT_R32G32_SFLOAT;
        attrDescs[1].offset   = offsetof(Vertex2D, uv);

    VkVertexInputBindingDescription bindingDesc = {};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex2D);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount = 1;
        vertexInput.pVertexBindingDescriptions = &bindingDesc;
        vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
        vertexInput.pVertexAttributeDescriptions = attrDescs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo  dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

    VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = NULL;   // dynamic
        viewportState.scissorCount = 1;
        viewportState.pScissors = NULL;    // dynamic

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample = {};
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttach = {};
        blendAttach.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        blendAttach.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlend = {};
        colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlend.attachmentCount = 1;
        colorBlend.pAttachments = &blendAttach;

    std::vector<VkDescriptorSetLayoutBinding> bindings(2);
        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        bindings[1].binding = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[1].descriptorCount = 1;
        bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
        setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        setLayoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    vkCreateDescriptorSetLayout(device, &setLayoutInfo, NULL, &descriptorSetLayout);

    VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(device, &layoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        std::cerr << "ERROR::VULKAN_SHADER::BUILD::PIPELINE_LAYOUT_CREATION_FAILED" << std::endl;
        return;
    }

    VkPipelineRenderingCreateInfo pipelineRendering = {};
        pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRendering.colorAttachmentCount = 1;
        pipelineRendering.pColorAttachmentFormats = &ctx.getSurfaceFormat().format;
        pipelineRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;   // or VK_FORMAT_UNDEFINED
        pipelineRendering.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &pipelineRendering;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.subpass = 0;
        pipelineInfo.pVertexInputState   = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pDynamicState       = &dynamicState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisample;
        pipelineInfo.pColorBlendState    = &colorBlend;
        pipelineInfo.pDepthStencilState  = &depthStencil;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &this->pipeline) != VK_SUCCESS) {
        std::cerr << "ERROR::VULKAN_SHADER::BUILD::GRAPHICS_PIPELINE_CREATION_FAILED" << std::endl;
        return;
    }

    vkDestroyShaderModule(device, fragModule, nullptr);
    vkDestroyShaderModule(device, vertModule, nullptr);

}


void Shader::bind() {
    VkCommandBuffer cmdBuffer = static_cast<RendererBackend&>(RaeptorCogs::Renderer().getBackend()).getRenderPipeline().getVulkanFrame().commandBuffer;
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void Shader::unbind() const {

}

void Shader::setBool(const std::string &name, bool value) const {
    (void)name;
    (void)value;
}

void Shader::setInt(const std::string &name, int value) const {
    (void)name;
    (void)value;
}

void Shader::setUInt(const std::string &name, unsigned int value) const {
    (void)name;
    (void)value;
}

void Shader::setFloat(const std::string &name, float value) const {
    (void)name;
    (void)value;
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    (void)name;
    (void)value;
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    (void)name;
    (void)value;
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    (void)name;
    (void)value;
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    (void)name;
    (void)mat;
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    (void)name;
    (void)mat;
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    (void)name;
    (void)mat;
}

static const std::vector<std::pair<Common::ShaderStage, size_t>> STAGES_MAP = {
    {Common::ShaderStage::VERTEX, RGSL_VERTEX},
    {Common::ShaderStage::FRAGMENT, RGSL_FRAGMENT},
    {Common::ShaderStage::GEOMETRY, RGSL_UNKNOWN_STAGE},
    {Common::ShaderStage::COMPUTE, RGSL_COMPUTE},
    {Common::ShaderStage::TESSELLATION_CONTROL, RGSL_UNKNOWN_STAGE},
    {Common::ShaderStage::TESSELLATION_EVALUATION, RGSL_UNKNOWN_STAGE}
};

const void * FindEmbeddedShader(const std::string& name, Common::ShaderStage stage, const std::string& profile) {
    return static_cast<const void *>(Common::FindEmbeddedShader(rgsl_shaders, sizeof(rgsl_shaders[0]), sizeof(rgsl_shaders) / sizeof(rgsl_shaders[0]), name, stage, STAGES_MAP, profile));
}

}
#endif // VULKAN_SUPPORTED