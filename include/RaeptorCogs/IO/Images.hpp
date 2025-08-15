#pragma once
#include <RaeptorCogs/IO/FileIO.hpp>
#include <glad/glad.h>
#include <iostream>
namespace RaeptorCogs {
class Image {
    public:
        std::unique_ptr<unsigned char[], void(*)(void*)> data;
        int width;
        int height;
        int channels;
        Image();
        Image(unsigned char* data, int width, int height, int channels);

        bool isOpaque() const {
            if (!data || channels < 4) return true; // Not enough channels for opacity
            for (int i = 0; i < width * height * channels; i += channels) {
                if (data[i + 3] < 255) {
                    return false; // If any pixel is not fully opaque
                }
            }
            return true; // All pixels are fully opaque
        }
};

GLuint LoadTexture(const char* filename);
Image LoadImageFromURL(const char* url);
Image LoadImageFromMemory(const FileData& data);
Image LoadImageFromFile(const char* filename);

}