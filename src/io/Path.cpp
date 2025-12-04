#include <RaeptorCogs/IO/Path.hpp>

namespace RaeptorCogs {

URLParts ParseURL(const std::string_view& url) {
    URLParts result;

    // Find "://"
    size_t protocol_pos = url.find("://");
    if (protocol_pos == std::string_view::npos)
        return result; // invalid URL
    
    size_t domain_start = protocol_pos + 3;

    // Find next '/'
    size_t path_pos = url.find('/', domain_start);

    if (path_pos == std::string_view::npos) {
        result.protocol = std::string(url.substr(0, protocol_pos));
        result.domain = std::string(url.substr(domain_start));   // no path
        result.path = "/";
    } else {
        result.protocol = std::string(url.substr(0, protocol_pos));
        result.domain = std::string(url.substr(domain_start, path_pos - domain_start));
        result.path   = std::string(url.substr(path_pos));
    }

    return result;
}

}