#include <RaeptorCogs/External/miniz/miniz.hpp>
#include <RaeptorCogs/Serializable.hpp>


namespace RaeptorCogs {



std::vector<uint8_t> Serializable::dump(SerializationFlag flags) {
    std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);
    Visitor serializer(ss, VisitorMode::SERIALIZE);
    reflect(serializer);

    switch(flags) {
        case SerializationFlag::COMPRESS:
        {
            uLongf compressed_size = compressBound(static_cast<uLongf>(ss.tellp()));
            std::vector<uint8_t> out(compressed_size);

            if (compress(out.data(), &compressed_size, (const Bytef*)ss.str().data(), static_cast<uLongf>(ss.tellp())) != Z_OK)
                throw std::runtime_error("compression failed");

            out.resize(compressed_size);
            return out;
        }
        case SerializationFlag::NONE:
        default:
        {
            std::string str = ss.str();
            return std::vector<uint8_t>(str.begin(), str.end());
        }
    }
}

void Serializable::load(std::vector<uint8_t> data, SerializationFlag flags) {
    std::vector<uint8_t> out;

    switch (flags) {
        case SerializationFlag::COMPRESS:
        {
            out.resize(data.size() * 4); // Start with a buffer 4 times the size of the compressed data
            uLongf expected_size = static_cast<uLongf>(out.size());

            while (uncompress(out.data(), (uLongf*)&expected_size, (const Bytef*)data.data(), static_cast<uLongf>(data.size())) != Z_OK) {
                out.resize(out.size() * 2); // Double the buffer size and try again
                expected_size = static_cast<uLongf>(out.size());
                if (out.size() > 100 * 1024 * 1024) // Arbitrary limit to prevent infinite loops
                    throw std::runtime_error("decompression failed: data too large");
            }
        }
            break;
        case SerializationFlag::NONE:
        default:
            out = std::move(data);
            break;
    }

    std::stringstream in(std::string((char*)out.data(), out.size()), std::ios::in | std::ios::binary);
    Visitor deserializer(in, VisitorMode::DESERIALIZE);
    reflect(deserializer);
}

}