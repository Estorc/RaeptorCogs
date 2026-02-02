struct InstanceGPUData { // Static SSBO structure
    mat4 model;
    vec4 uv;
    int type;
    int dataOffset;
    int writeMaskID;
    int readMaskID;
};