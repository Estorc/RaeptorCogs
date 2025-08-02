#include <iostream>
#include <fstream>

// Helper function to load a file into memory
unsigned char* load_file(const char* filename) {
    std::ifstream file;
    file.open(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return nullptr;
    }  
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (size <= 0) {
        std::cerr << "File is empty or invalid: " << filename << std::endl;
        return nullptr;
    }
    unsigned char* buffer = new unsigned char[size + 1]; // +1 for null terminator
    if (!buffer) {
        std::cerr << "Failed to allocate memory for file: " << filename << std::endl;
        return nullptr;
    }
    file.read(reinterpret_cast<char*>(buffer), size);
    if (!file) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        delete[] buffer;
        return nullptr;
    }
    file.close();
    buffer[size] = '\0'; // Null-terminate the buffer
    return buffer;
}


void separate_domain_and_path(const std::string& url, std::string& domain, std::string& path) {
    std::string url_str(url);
    size_t protocol_end = url_str.find("://");
    if (protocol_end != std::string::npos) {
        size_t domain_start = protocol_end + 3;
        size_t path_start = url_str.find('/', domain_start);
        if (path_start != std::string::npos) {
            domain = url_str.substr(domain_start, path_start - domain_start);
            path = url_str.substr(path_start);
        } else {
            domain = url_str.substr(domain_start);
            path = "/";
        }
    } else {
        // URL doesn't have protocol
        size_t path_start = url_str.find('/');
        if (path_start != std::string::npos) {
            domain = url_str.substr(0, path_start);
            path = url_str.substr(path_start);
        } else {
            domain = url_str;
            path = "/";
        }
    }
}
