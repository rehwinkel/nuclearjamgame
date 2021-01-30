#include "resource.h"

#include <fstream>
#include <zlib.h>

std::vector<char> decompress_data(const std::vector<char>& compr_data,
                                  uint32_t raw_size) {
    uLong _raw_size = raw_size;
    std::vector<char> raw_data(raw_size);
    if (uncompress((uint8_t*)raw_data.data(), &_raw_size,
                   (uint8_t*)compr_data.data(),
                   (uLong)compr_data.size()) != Z_OK) {
        panic("failed to decompress data");
    }
    if (_raw_size != raw_size) {
        panic("decompressed data size doesnt match");
    }
    return raw_data;
}

Resource Resource::deserialize(const std::vector<char>& data) {
    uint32_t pos = 0;
    Serializer<uint32_t> atlas_size_de;
    uint32_t atlas_width =
        atlas_size_de.deserialize(data.data(), pos);  // width
    uint32_t atlas_height =
        atlas_size_de.deserialize(data.data(), pos);  // height
    Serializer<std::vector<char>> atlas_data_de;
    std::vector<char> atlas_data =
        atlas_data_de.deserialize(data.data(), pos);  // atlas
    Serializer<std::vector<AtlasTexture>> atlas_tex_de;
    std::vector<AtlasTexture> textures =
        atlas_tex_de.deserialize(data.data(), pos);  // textures
    Serializer<std::vector<std::vector<char>>> other_data_de;
    std::vector<std::vector<char>> other_data =
        other_data_de.deserialize(data.data(), pos);  // other
    return Resource(textures, atlas_width, atlas_height, atlas_data,
                    other_data);
}

Resource Resource::from_file(const char* in_file_path) {
    std::ifstream file(in_file_path);
    if (!file.is_open()) {
        panic("failed to open source file");
    }

    uint32_t compressed_size;
    file.read((char*)&compressed_size, sizeof(compressed_size));
    uint32_t raw_size;
    file.read((char*)&raw_size, sizeof(raw_size));
    std::vector<char> compressed_data(compressed_size);
    file.read(compressed_data.data(), compressed_size);
    std::vector<char> raw_data = decompress_data(compressed_data, raw_size);
    return Resource::deserialize(raw_data);
}

Resource Resource::from_buffer(const char* data, uint32_t size) {
    uint32_t compressed_size = *(uint32_t*)data;
    uint32_t raw_size = *(uint32_t*)(data + sizeof(compressed_size));
    std::vector<char> compressed_data(
        data + sizeof(compressed_size) + sizeof(raw_size),
        data + sizeof(compressed_size) + sizeof(raw_size) + size);
    std::vector<char> raw_data = decompress_data(compressed_data, raw_size);
    return Resource::deserialize(raw_data);
}