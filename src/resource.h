#pragma once

#include "util.h"
#include <vector>
#include <iostream>

struct AtlasTexture {
    uint16_t x, y, width, height;
    AtlasTexture();
    AtlasTexture(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
};

struct Atlas {
    uint32_t width, height;
    std::vector<char>& data;
};

class Resource {
    uint32_t atlas_width;
    uint32_t atlas_height;
    std::vector<char> atlas_data;
    std::vector<AtlasTexture> textures;
    std::vector<std::vector<char>> other;

    Resource(std::vector<AtlasTexture> textures, uint32_t width,
             uint32_t height, std::vector<char> data,
             std::vector<std::vector<char>> other);
    std::vector<char> serialize();
    static Resource deserialize(const std::vector<char>& data);

   public:
    static Resource from_file(const char* path);
    static Resource from_buffer(const char* data, uint32_t size);
    void to_file(const char* path);
    std::vector<char> to_buffer();
    Atlas get_atlas();
    const AtlasTexture& get_texture(uint32_t index);
    const std::vector<char>& get_other(uint32_t index);
    friend class ResourceBuilder;
};

struct RawTexture {
    std::vector<char> data;
    uint32_t width;
    uint32_t height;
};

class ResourceBuilder {
    std::vector<RawTexture> textures;
    std::vector<std::vector<char>> other;

   public:
    ResourceBuilder();
    uint32_t add_texture(const char* path);
    uint32_t add_other(const char* path);
    Resource build();
};

template <class T>
class Serializer {
   public:
    void serialize(std::ostream& stream, const T& data) {
        stream.write((char*)&data, sizeof(T));
    }
    T deserialize(const char* ptr, uint32_t& pos) {
        const char* actual = ptr + pos;
        pos += sizeof(T);
        return *(T*)actual;
    }
};

template <class T>
class Serializer<std::vector<T>> {
   public:
    void serialize(std::ostream& stream, const std::vector<T>& data) {
        uint32_t len = data.size();
        stream.write((char*)&len, sizeof(len));
        Serializer<T> ser;
        for (const T& el : data) {
            ser.serialize(stream, el);
        }
    }
    std::vector<T> deserialize(const char* ptr, uint32_t& pos) {
        Serializer<uint32_t> len_de;
        uint32_t len = len_de.deserialize(ptr, pos);
        std::vector<T> result;
        result.reserve(len);
        Serializer<T> el_de;
        for (uint32_t i = 0; i < len; i++) {
            result.emplace_back(el_de.deserialize(ptr, pos));
        }
        return result;
    }
};