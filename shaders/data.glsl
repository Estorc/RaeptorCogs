layout(std430, binding = 0) buffer RawDataBuffer {
    uint data[];
};

float unpackFloat(uint indexBytes)
{
    // Rebuild 4 bytes from 4 * 8-bit offsets
    uint byte0 = data[indexBytes / 4] >> ((indexBytes % 4) * 8) & 0xFFu;
    uint byte1 = data[(indexBytes + 1) / 4] >> (((indexBytes + 1) % 4) * 8) & 0xFFu;
    uint byte2 = data[(indexBytes + 2) / 4] >> (((indexBytes + 2) % 4) * 8) & 0xFFu;
    uint byte3 = data[(indexBytes + 3) / 4] >> (((indexBytes + 3) % 4) * 8) & 0xFFu;

    uint combined = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    return uintBitsToFloat(combined);
}

vec3 unpackVec3(uint indexBytes)
{
    // Rebuild 3 floats from 12 * 8-bit offsets
    float x = unpackFloat(indexBytes);
    float y = unpackFloat(indexBytes + 4);
    float z = unpackFloat(indexBytes + 8);
    return vec3(x, y, z);
}