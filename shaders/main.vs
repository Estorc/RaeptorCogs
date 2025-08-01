#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instanceModel;
layout (location = 6) in vec4 instanceUV;
layout (location = 7) in uint instanceType; // Instance type
layout (location = 8) in uint instanceDataOffset; // Offset into another SSBO or same buffer

out vec2 TexCoord;
out flat uint Type; // Instance type
out flat uint DataOffset; // Offset into another SSBO or same buffer
out vec4 UVOffset; // Offset for UV coordinates

uniform mat4 projection;

void main() {
    gl_Position = projection * instanceModel * vec4(aPos, 0.0, 1.0);
    TexCoord = instanceUV.xy + aTexCoord * instanceUV.zw;
    Type = instanceType;
    DataOffset = instanceDataOffset;
    UVOffset = instanceUV;
}