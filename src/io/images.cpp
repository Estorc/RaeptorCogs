#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/Images.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#ifndef __EMSCRIPTEN__
#include <httplib.h>
#endif
namespace RaeptorCogs {

Image::Image() : data(nullptr, stbi_image_free), width(0), height(0), channels(0) {}
Image::Image(unsigned char* data, int width, int height, int channels) : data(data, stbi_image_free), width(width), height(height), channels(channels) {}

GLuint LoadTexture(const char* filename) {
    GLuint textureID = 0;
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    return textureID;
}

Image LoadImageFromURL(const char* url) {
    #ifndef __EMSCRIPTEN__
    // Parse URL to extract domain and path
    std::string domain, path;
    separate_domain_and_path(url, domain, path);

    httplib::Client client(domain.c_str());
    client.set_follow_location(true); // Follow redirects if needed
    auto res = client.Get(path.c_str());
    if (!res || res->status != 200) {
        std::cerr << "Failed to load image from URL: " << url << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res->body.data()), res->body.size(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to decode image from URL: " << url << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    return Image(data, width, height, channels);
    #else
    return Image(nullptr, 0, 0, 0);
    #endif
}

Image LoadImageFromMemory(const FileData& data) {
    int width, height, channels;
    unsigned char* img_data = stbi_load_from_memory(data.data(), data.size(), &width, &height, &channels, 4);
    if (!img_data) {
        std::cerr << "Failed to load image from memory" << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    return Image(img_data, width, height, channels);
}

Image LoadImageFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    return Image(data, width, height, channels);
}

}