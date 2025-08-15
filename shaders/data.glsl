layout(std430, binding = 0) buffer RawDataBuffer {
    float data[];
};

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
