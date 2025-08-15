#version 300 es
precision highp float;
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instanceModel;
layout (location = 6) in vec4 instanceUV;
layout (location = 7) in int instanceType; // Instance type
layout (location = 8) in int instanceDataOffset; // Offset into another SSBO or same buffer

flat out int Type; // Instance type
flat out int DataOffset; // Offset into another SSBO or same buffer
out vec2 TexCoord;
out vec4 UVOffset; // Offset for UV coordinates

uniform mat4 projection;

void main() {
    if (instanceType == 0) {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }
    gl_Position = projection * instanceModel * vec4(aPos, 0.0, 1.0);
    TexCoord = instanceUV.xy + aTexCoord * instanceUV.zw;
    UVOffset = instanceUV;
    Type = instanceType;
    DataOffset = instanceDataOffset;
}