#version 460 core

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D uTextureSampler;

void main() {
    outColor = texture(uTextureSampler, vUV);
}
