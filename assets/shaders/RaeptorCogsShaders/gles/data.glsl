#version 300 es
#include <RaeptorCogsShaders/common/data.glsl>
uniform usampler2D uIndirectionBuffer;
uniform usampler2D uInstanceBuffer;
uniform usampler2D uRawDataBuffer;
uniform int uBaseInstance;


ivec2 texelCoord(int linearIndex)
{
    return ivec2(
        linearIndex % IDATATEX_WIDTH,
        linearIndex / IDATATEX_WIDTH
    );
}


int getBaseInstance() {
    return uBaseInstance;
}


int getInstanceID(int baseInstance, int instanceID) {
    int linearIndex = instanceID + baseInstance;
    uvec4 texel = texelFetch(uIndirectionBuffer, texelCoord(linearIndex / 4), 0);
    switch (linearIndex % 4) {
        case 0: return int(texel.r);
        case 1: return int(texel.g);
        case 2: return int(texel.b);
        case 3: return int(texel.a);
    }
}

const int INSTANCE_TEXELS = 6;
InstanceGPUData getInstanceData(int instanceID) {
    int base = instanceID * INSTANCE_TEXELS;

    uvec4 t0 = texelFetch(uInstanceBuffer, texelCoord(base + 0), 0);
    uvec4 t1 = texelFetch(uInstanceBuffer, texelCoord(base + 1), 0);
    uvec4 t2 = texelFetch(uInstanceBuffer, texelCoord(base + 2), 0);
    uvec4 t3 = texelFetch(uInstanceBuffer, texelCoord(base + 3), 0);
    uvec4 t4 = texelFetch(uInstanceBuffer, texelCoord(base + 4), 0);
    uvec4 t5 = texelFetch(uInstanceBuffer, texelCoord(base + 5), 0);

    InstanceGPUData instance;

    instance.model[0] = vec4(
        uintBitsToFloat(t0.r),
        uintBitsToFloat(t0.g),
        uintBitsToFloat(t0.b),
        uintBitsToFloat(t0.a)
    );

    instance.model[1] = vec4(
        uintBitsToFloat(t1.r),
        uintBitsToFloat(t1.g),
        uintBitsToFloat(t1.b),
        uintBitsToFloat(t1.a)
    );

    instance.model[2] = vec4(
        uintBitsToFloat(t2.r),
        uintBitsToFloat(t2.g),
        uintBitsToFloat(t2.b),
        uintBitsToFloat(t2.a)
    );

    instance.model[3] = vec4(
        uintBitsToFloat(t3.r),
        uintBitsToFloat(t3.g),
        uintBitsToFloat(t3.b),
        uintBitsToFloat(t3.a)
    );

    instance.uv = vec4(
        uintBitsToFloat(t4.r),
        uintBitsToFloat(t4.g),
        uintBitsToFloat(t4.b),
        uintBitsToFloat(t4.a)
    );

    instance.type        = int(t5.r);
    instance.dataOffset  = int(t5.g);
    instance.writeMaskID = int(t5.b);
    instance.readMaskID  = int(t5.a);

    return instance;
}

float unpackFloat(int texelIndex) {
    uvec4 texel = texelFetch(uRawDataBuffer, texelCoord(texelIndex/4), 0);
    switch (texelIndex % 4) {
        case 0: return uintBitsToFloat(texel.r);
        case 1: return uintBitsToFloat(texel.g);
        case 2: return uintBitsToFloat(texel.b);
        case 3: return uintBitsToFloat(texel.a);
    }
}

vec3 unpackVec3(int texelIndex) {
    uvec4 t0 = texelFetch(uRawDataBuffer, texelCoord(texelIndex/4 + 0), 0);
    uvec4 t1 = texelFetch(uRawDataBuffer, texelCoord(texelIndex/4 + 1), 0);

    switch (texelIndex % 4) {
        case 0: return vec3(
            uintBitsToFloat(t0.r),
            uintBitsToFloat(t0.g),
            uintBitsToFloat(t0.b)
        );
        case 1: return vec3(
            uintBitsToFloat(t0.g),
            uintBitsToFloat(t0.b),
            uintBitsToFloat(t0.a)
        );
        case 2: return vec3(
            uintBitsToFloat(t0.b),
            uintBitsToFloat(t0.a),
            uintBitsToFloat(t1.r)
        );
        case 3: return vec3(
            uintBitsToFloat(t0.a),
            uintBitsToFloat(t1.r),
            uintBitsToFloat(t1.g)
        );
    }
}