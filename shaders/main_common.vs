layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec2 vertexUV;

struct InstanceGPUData { // Static SSBO structure
    mat4 model;
    vec4 uv;
    int type;
    int dataOffset;
    int writeMaskID;
    int readMaskID;
};

layout(std430, binding = 0) readonly buffer IndirectionBuffer {
    int instanceIndices[];
};

layout(std430, binding = 1) readonly buffer InstanceBuffer {
    InstanceGPUData instances[];
};

out vec2 vUV;
out vec3 vBarycentric; // Normal for UV mapping
out vec4 vClipPos;
flat out int Type; // Instance type
flat out int DataOffset; // Offset into another SSBO or same buffer
flat out int readMaskID; // Reading mask ID
flat out int writeMaskID; // Writing mask ID

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform float uTime; 

void main() {
    InstanceGPUData instance = instances[instanceIndices[gl_InstanceID + gl_BaseInstance]];
    if (instance.type == 0) {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        return;
    }

    vClipPos = uProjectionMatrix * uViewMatrix * instance.model * vec4(vertexPos, 0.0, 1.0);
    gl_Position = vClipPos; // Slight animation for testing
    //gl_Position.xy += sin(uTime + float(gl_InstanceID)) * 0.01;
    Type = instance.type;

    // UV coordinates
    vUV = instance.uv.xy + vertexUV * instance.uv.zw;

    // Assign barycentric coordinates
    /*if (vertexID == 0) vBarycentric = vec3(1,0,0);
    else if (vertexID == 1) vBarycentric = vec3(0,1,0);
    else vBarycentric = vec3(0,0,1);*/
    vBarycentric = vec3(1.0); // Not used for now

    DataOffset = instance.dataOffset;
    readMaskID = instance.readMaskID;
    writeMaskID = instance.writeMaskID;
}