#ifdef GL_ES
#define LAYOUT(x)
#else
#define LAYOUT(x) layout(x)
#endif

LAYOUT(location = 0) in vec2 vUV;
LAYOUT(location = 1) in vec3 vBarycentric;
LAYOUT(location = 2) in vec4 vClipPos;
LAYOUT(location = 3) flat in int Type;
LAYOUT(location = 4) flat in int DataOffset;
LAYOUT(location = 5) flat in int readMaskID;
LAYOUT(location = 6) flat in int writeMaskID;
LAYOUT(location = 0) out uvec4 FragMask;

LAYOUT(binding = 0) uniform sampler2D uTextureSampler;
LAYOUT(binding = 1) uniform usampler2D uMaskTextureSampler;

void main() {
    uint maskValue = texture(uMaskTextureSampler, vClipPos.xy / vClipPos.w * 0.5 + 0.5).r;
    if (maskValue != uint(readMaskID)) {
        discard; // Discard transparent fragments
    }
    float edgeWidth = 1.0; // Adjust this value to change edge thickness
    vec3 d = fwidth(vBarycentric); 
    float minB = min(min(vBarycentric.x, vBarycentric.y), vBarycentric.z);
    float edgeFactor = smoothstep(0.0, max(d.x,d.y)*edgeWidth, minB);
    //edgeFactor = 1.0;
    vec4 outlineColor = vec4(0.0, 1.0, 0.0, 1.0); // Outline color (green)
    vec4 fillColor; // Fill color (black)
    switch (Type) {
        case RENDERER_MODE_2D_SPRITE:
            // Default rendering behavior
            fillColor.rgb = unpackVec3(DataOffset);
            fillColor = texture(uTextureSampler, vUV) * vec4(fillColor.rgb, 1.0);
            break;
        case RENDERER_MODE_2D_TEXT:
            // Custom rendering behavior for mode 2
            fillColor.rgb = unpackVec3(DataOffset);
            float smoothing = unpackFloat(DataOffset + 3);
            float dist = texture(uTextureSampler, vUV).r; // 0..1
            // Map distance around 0.5 = glyph edge
            float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
            fillColor = vec4(fillColor.rgb, alpha);
            break;
        default:
            // Fallback for any other mode
            fillColor = texture(uTextureSampler, vUV) * vec4(1.0);
    }
    if (fillColor.a < 0.5) discard;
    FragMask = uvec4(writeMaskID, 0, 0, 0);
}