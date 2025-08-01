#pragma once
#include <glad/gl.h>
#include <iostream>

typedef struct Image {
    unsigned char* data;
    int width;
    int height;
    int channels;
} Image;

GLuint load_texture(const char* filename);
Image load_image(const char* filename);