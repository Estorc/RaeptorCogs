#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#include <RaeptorLab/io/string.hpp>
#include <RaeptorLab/io/images.hpp>
#include <httplib.h>

GLuint load_texture(const char* filename) {
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

Image load_image_from_url(const char* url) {
    // Parse URL to extract domain and path
    std::string domain, path;
    separate_domain_and_path(url, domain, path);

    httplib::Client client(domain.c_str());
    client.set_follow_location(true); // Follow redirects if needed
    auto res = client.Get(path.c_str());
    if (!res || res->status != 200) {
        std::cerr << "Failed to load image from URL: " << url << std::endl;
        return {nullptr, 0, 0, 0};
    }
    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res->body.data()), res->body.size(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to decode image from URL: " << url << std::endl;
        return {nullptr, 0, 0, 0};
    }
    return {data, width, height, channels};
}

Image load_image(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
    }
    return {data, width, height, channels};
}