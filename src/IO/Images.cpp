#include <RaeptorCogs/External/stb/stb.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/IO/Images.hpp>
#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/IO/Path.hpp>
#include <RaeptorCogs/External/glad/glad.hpp>
#include <cstring>
#ifndef __EMSCRIPTEN__
//#include <httplib.h>
#endif
namespace RaeptorCogs {

Image::Image() : data(nullptr, stbi_image_free), width(0), height(0), channels(0) {}
Image::Image(unsigned char* data, size_t width, size_t height, size_t channels) : data(data, stbi_image_free), width(width), height(height), channels(channels) {}

GLuint LoadTexture(const std::filesystem::path& filename) {
    GLuint textureID = 0;
    int width, height, channels;
    unsigned char* data = stbi_load(filename.string().c_str(), &width, &height, &channels, 4);

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

Image LoadImageFromURL(const std::filesystem::path& url) {
/*    #ifndef __EMSCRIPTEN__
    // Parse URL to extract domain and path
    auto [domain, path] = ParseURL(url.string());
    httplib::Client client(domain.c_str());
    client.set_follow_location(true); // Follow redirects if needed
    auto res = client.Get(path.c_str());
    if (!res || res->status != 200) {
        std::cerr << "Failed to load image from URL: " << url << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res->body.data()), static_cast<int>(res->body.size()), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to decode image from URL: " << url << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    return Image(data, static_cast<size_t>(width), static_cast<size_t>(height), static_cast<size_t>(channels));
    #else
    return Image(nullptr, 0, 0, 0);
    #endif*/ (void) url; return Image(nullptr, 0, 0, 0);
}

Image LoadImageFromMemory(const FileData& filedata, size_t s_width, size_t s_height) {
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(filedata.data(), static_cast<int>(filedata.size()), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image from memory" << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    if (s_width > 0 || s_height > 0) {
        if (s_width <= 0) s_width = (static_cast<size_t>(width) * s_height) / static_cast<size_t>(height);
        if (s_height <= 0) s_height = (static_cast<size_t>(height) * s_width) / static_cast<size_t>(width);
        unsigned char* resized_data = new unsigned char[s_width * s_height * 4];
        stbir_resize_uint8_linear(data, width, height, 0, resized_data, static_cast<int>(s_width), static_cast<int>(s_height), 0, STBIR_RGBA);
        stbi_image_free(data);
        return Image(resized_data, s_width, s_height, 4);
    }
    return Image(data, static_cast<size_t>(width), static_cast<size_t>(height), static_cast<size_t>(channels));
}

Image LoadImageFromFile(const std::filesystem::path& filename, size_t s_width, size_t s_height) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.string().c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        
        return Image(nullptr, 0, 0, 0);
    }
    if (s_width > 0 || s_height > 0) {
        if (s_width <= 0) s_width = (static_cast<size_t>(width) * s_height) / static_cast<size_t>(height);
        if (s_height <= 0) s_height = (static_cast<size_t>(height) * s_width) / static_cast<size_t>(width);
        unsigned char* resized_data = new unsigned char[s_width * s_height * 4];
        stbir_resize_uint8_linear(data, width, height, 0, resized_data, static_cast<int>(s_width), static_cast<int>(s_height), 0, STBIR_RGBA);
        stbi_image_free(data);
        return Image(resized_data, s_width, s_height, 4);
    }
    return Image(data, static_cast<size_t>(width), static_cast<size_t>(height), static_cast<size_t>(channels));
}

Image CreateImage(size_t width, size_t height) {
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid image dimensions." << std::endl;
        return Image(nullptr, 0, 0, 0);
    }
    unsigned char* data = new unsigned char[width * height * 4]();
    return Image(data, width, height, 4);
}

void saveTextureToPNG(GLuint textureID, size_t width, size_t height, const std::filesystem::path& filename) {
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate CPU buffer (RGBA8)
    std::vector<unsigned char> pixels(width * height * 4);

    // Read texture data
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Flip vertically because OpenGL's origin is bottom-left
    std::vector<unsigned char> flipped(width * height * 4);
    for (size_t y = 0; y < height; ++y) {
        memcpy(&flipped[y * width * 4],
               &pixels[(height - 1 - y) * width * 4],
               width * 4);
    }

    // Write PNG
    if (!stbi_write_png(filename.string().c_str(), static_cast<int>(width), static_cast<int>(height), 4, flipped.data(), static_cast<int>(width) * 4)) {
        std::cerr << "Failed to save PNG: " << filename << std::endl;
    } else {
        std::cout << "Saved PNG: " << filename << std::endl;
    }
}


}