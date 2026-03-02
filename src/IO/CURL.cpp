#ifdef RAEPTOR_HAS_CURL
#include <RaeptorCogs/External/CURL/CURL.hpp>
#include <RaeptorCogs/IO/CURL.hpp>
#include <iostream>

namespace RaeptorCogs::CURL {

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t totalSize     = size * nmemb;
  MemoryBuffer *buffer = static_cast<MemoryBuffer *>(userp);
  buffer->insert(
      buffer->end(), (unsigned char *)contents, (unsigned char *)contents + totalSize);
  return totalSize;
}

std::unique_ptr<MemoryBuffer> LoadDataFromURL(const std::string &url) {
  ::CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialize curl\n";
    return nullptr;
  }

  MemoryBuffer buffer;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow redirects
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
    curl_easy_cleanup(curl);
    return nullptr;
  }

  long response_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  curl_easy_cleanup(curl);

  if (response_code != 200) {
    std::cerr << "HTTP error: " << response_code << "\n";
    return nullptr;
  }

  return std::make_unique<MemoryBuffer>(std::move(buffer));
}

} // namespace RaeptorCogs::CURL
#endif
