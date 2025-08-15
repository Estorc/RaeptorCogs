#version 300 es
precision highp float;

#include <constants.glsl>
#include <data_gles.glsl>

flat in int Type;
flat in int DataOffset;
in vec2 TexCoord;
in vec4 UVOffset;
out vec4 FragColor;

uniform sampler2D Texture;
uniform float Time;

void main() {
    vec3 col;
    switch (Type) {
        case RENDERER_MODE_2D_SPRITE:
            // Default rendering behavior
            col = unpackVec3(DataOffset);
            FragColor = texture(Texture, TexCoord) * vec4(col, 1.0);
            break;
        case RENDERER_MODE_2D_TEXT:
            // Custom rendering behavior for mode 2
            col = unpackVec3(DataOffset);
            FragColor = vec4(vec3(col), texture(Texture, TexCoord).r);
            break;
        default:
            // Fallback for any other mode
            FragColor = texture(Texture, TexCoord) * vec4(1.0);
    }
}