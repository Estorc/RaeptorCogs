#pragma once
#include <string>

unsigned char* load_file(const char* filename);
void separate_domain_and_path(const std::string& url, std::string& domain, std::string& path);
size_t utf8_char_length(const unsigned char* c);