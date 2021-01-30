#include "resource.h"

#include <sstream>
#include <fstream>
#include <cstring>
#include <zlib.h>

std::vector<char> compress_data(const std::vector<char>& raw_data) {
    uLong compressed_size = compressBound(raw_data.size());
    std::vector<char> compressed(compressed_size);
    if (compress2((uint8_t*)compressed.data(), &compressed_size,
                  (uint8_t*)raw_data.data(), (uLong)raw_data.size(),
                  Z_BEST_COMPRESSION) != Z_OK) {
        panic("failed to compress data");
    }
    compressed.resize(compressed_size);
    return compressed;
}

std::vector<char> Resource::serialize() {
    std::ostringstream ss;
    Serializer<uint32_t> atlas_size_ser;
    atlas_size_ser.serialize(ss, this->atlas_width);   // width
    atlas_size_ser.serialize(ss, this->atlas_height);  // height
    Serializer<std::vector<char>> atlas_data_ser;
    atlas_data_ser.serialize(ss, this->atlas_data);  // atlas
    Serializer<std::vector<AtlasTexture>> atlas_tex_ser;
    atlas_tex_ser.serialize(ss, this->textures);  // textures
    Serializer<std::vector<std::vector<char>>> other_data_ser;
    other_data_ser.serialize(ss, this->other);  // other
    std::string s_data = ss.str();
    std::vector<char> data(s_data.data(), s_data.data() + s_data.size());
    return data;
}

void Resource::to_file(const char* path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        panic("failed to open destination file");
    }
    std::vector<char> data = this->serialize();
    std::vector<char> compressed = compress_data(data);
    uint32_t compressed_size = compressed.size();
    uint32_t raw_size = data.size();
    file.write((char*)&compressed_size, sizeof(compressed_size));
    file.write((char*)&raw_size, sizeof(raw_size));
    file.write(compressed.data(), compressed_size);
}

std::vector<char> Resource::to_buffer() {
    std::vector<char> data = this->serialize();
    std::vector<char> compressed = compress_data(data);
    uint32_t compressed_size = compressed.size();
    uint32_t raw_size = data.size();

    std::vector<char> result(compressed_size + sizeof(compressed_size) +
                             sizeof(raw_size));
    *(uint32_t*)result.data() = compressed_size;
    *(uint32_t*)(result.data() + sizeof(compressed_size)) = raw_size;
    char* data_dest =
        result.data() + sizeof(compressed_size) + sizeof(raw_size);
    std::memcpy(data_dest, compressed.data(), compressed_size);
    return result;
}