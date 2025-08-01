#include <iostream>
#include <fstream>

// Helper function to load a file into memory
unsigned char* load_file(const char* filename) {
    std::ifstream file;
    file.open(filename);
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