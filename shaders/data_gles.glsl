uniform sampler2D uIDataTexSampler;

float unpackFloat(int indexBytes)
{
    int texX = (indexBytes/4) % IDATATEX_WIDTH;   // compute texel coordinates
    int texY = (indexBytes/4) / IDATATEX_WIDTH;
    vec4 texel = texelFetch(uIDataTexSampler, ivec2(texX, texY), 0);
    switch (indexBytes % 4) {
        case 0: return texel.r; // each texel.r is a float
        case 1: return texel.g; // each texel.g is a float
        case 2: return texel.b; // each texel.b is a float
        default: return texel.a; // each texel.a is a float
    }
}

vec3 unpackVec3(int indexBytes)
{
    // Rebuild 3 floats from 12 * 8-bit offsets
    float x = unpackFloat(indexBytes);
    float y = unpackFloat(indexBytes + 1);
    float z = unpackFloat(indexBytes + 2);
    return vec3(x, y, z);
}