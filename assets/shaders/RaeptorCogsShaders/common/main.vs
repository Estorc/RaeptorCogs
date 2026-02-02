#ifdef GL_ES
#define LAYOUT(x)
#else
#define LAYOUT(x) layout(x)
#endif

LAYOUT(location = 0) in vec2 vertexPos;
LAYOUT(location = 1) in vec2 vertexUV;

LAYOUT(location = 0) out vec2 vUV;
LAYOUT(location = 1) out vec3 vBarycentric; // Normal for UV mapping
LAYOUT(location = 2) out vec4 vClipPos;
LAYOUT(location = 3) flat out int Type; // Instance type
LAYOUT(location = 4) flat out int DataOffset; // Offset into another SSBO or same buffer
LAYOUT(location = 5) flat out int readMaskID; // Reading mask ID
LAYOUT(location = 6) flat out int writeMaskID; // Writing mask ID

LAYOUT(location = 0) uniform mat4 uViewMatrix;
LAYOUT(location = 1) uniform mat4 uProjectionMatrix;
LAYOUT(location = 2) uniform float uTime; 

void main() {
    #if defined(HAVE_BASE_INSTANCE)
    InstanceGPUData instance = instances[instanceIndices[gl_InstanceID + gl_BaseInstance]];
    #else
    InstanceGPUData instance = getInstanceData(getInstanceID(getBaseInstance(), gl_InstanceID));
    #endif
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