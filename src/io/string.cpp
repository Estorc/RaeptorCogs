#include <iostream>
namespace RaeptorCogs {

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


size_t utf8_char_length(const unsigned char* c) {

    if (*c <= 0x7F) return 1;             // ASCII
    else if ((*c >> 5) == 0b110) return 2; // 2-byte char
    else if ((*c >> 4) == 0b1110) return 3; // 3-byte char
    else if ((*c >> 3) == 0b11110) return 4; // 4-byte char

    // Invalid UTF-8 start byte (for simplicity, treat as 1)
    return 1;
}

}