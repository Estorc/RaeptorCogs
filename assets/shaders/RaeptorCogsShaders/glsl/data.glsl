#version 460 core
#include <RaeptorCogsShaders/common/data.glsl>
layout(std430, binding = 0) readonly buffer IndirectionBuffer {
    int instanceIndices[];
};

layout(std430, binding = 1) readonly buffer InstanceBuffer {
    InstanceGPUData instances[];
};

layout(std430, binding = 2) buffer RawDataBuffer {
    float data[];
};


#define HAVE_BASE_INSTANCE

int getInstanceID(int baseInstance, int instanceID) {
    return instanceIndices[instanceID + baseInstance];
}

InstanceGPUData getInstanceData(int instanceID) {
    return instances[instanceIndices[instanceID]];
}

float unpackFloat(int indexFloats)
{
    // Now just retrieve the float directly
    return data[indexFloats];
}

vec3 unpackVec3(int indexFloats)
{
    // Retrieve 3 consecutive floats to form a vec3
    float x = data[indexFloats];
    float y = data[indexFloats + 1];
    float z = data[indexFloats + 2];
    return vec3(x, y, z);
}
