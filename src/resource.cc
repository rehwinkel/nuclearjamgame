#include "resource.h"

#include <memory>
#include <cstring>
#include <iterator>
#include <fstream>
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ResourceBuilder::ResourceBuilder() {}

uint32_t ResourceBuilder::add_texture(const char* path) {
    FILE* in_file = fopen(path, "rb");
    if (!in_file) {
        panic("failed to open texture file");
    }
    int32_t x, y, components;
    uint8_t* image_data = stbi_load_from_file(in_file, &x, &y, &components, 4);
    RawTexture result{
        std::vector<char>((char*)image_data, (char*)image_data + x * y * 4),
        (uint32_t)x, (uint32_t)y};
    stbi_image_free(image_data);
    this->textures.emplace_back(std::move(result));
    return this->textures.size() - 1;
}

uint32_t ResourceBuilder::add_other(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        panic("failed to open other file");
    }
    std::vector<char> content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    this->other.emplace_back(std::move(content));
    return this->other.size() - 1;
}

struct Node {
    bool used;
    uint32_t x, y, w, h;
    std::unique_ptr<Node> right, down;

    Node() : x(0), y(0), w(0), h(0), used(false) {}
    Node(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : x(x), y(y), w(w), h(h), used(false) {}

    Node* find_node(uint32_t width, uint32_t height) {
        if (this->used) {
            if (this->right) {
                Node* right = this->right->find_node(width, height);
                if (right) return right;
            }
            if (this->down) {
                Node* down = this->down->find_node(width, height);
                if (down) return down;
            }
        } else if (width <= this->w && height <= this->h) {
            return this;
        }
        return nullptr;
    }

    Node* split_node(uint32_t width, uint32_t height) {
        this->used = true;
        this->down = std::make_unique<Node>(x, y + height, w, h - height);
        this->right = std::make_unique<Node>(x + width, y, w - width, h);
        return this;
    }

    static Node* grow_node(std::unique_ptr<Node>& root, uint32_t width,
                           uint32_t height) {
        bool canGrowDown = (width <= root->w);
        bool canGrowRight = (height <= root->h);
        bool shouldGrowRight = canGrowRight && (root->h >= (root->w + width));
        bool shouldGrowDown = canGrowDown && (root->w >= (root->h + height));

        if (shouldGrowRight) {
            return Node::grow_right(root, width, height);
        } else if (shouldGrowDown) {
            return Node::grow_down(root, width, height);
        } else if (canGrowRight) {
            return Node::grow_right(root, width, height);
        } else if (canGrowDown) {
            return Node::grow_down(root, width, height);
        } else {
            return nullptr;
        }
    }

    static Node* grow_right(std::unique_ptr<Node>& root, uint32_t width,
                            uint32_t height) {
        std::unique_ptr<Node> new_root =
            std::make_unique<Node>(0, 0, root->w + width, root->h);
        new_root->used = true;
        new_root->right = std::make_unique<Node>(root->w, 0, width, root->h);
        new_root->down = std::move(root);
        root = std::move(new_root);

        Node* node = root->find_node(width, height);
        if (node) {
            return node->split_node(width, height);
        }
        return nullptr;
    }

    static Node* grow_down(std::unique_ptr<Node>& root, uint32_t width,
                           uint32_t height) {
        std::unique_ptr<Node> new_root =
            std::make_unique<Node>(0, 0, root->w, root->h + height);
        new_root->used = true;
        new_root->down = std::make_unique<Node>(0, root->h, root->w, height);
        new_root->right = std::move(root);
        root = std::move(new_root);

        Node* node = root->find_node(width, height);
        if (node) {
            return node->split_node(width, height);
        }
        return nullptr;
    }
};

Resource ResourceBuilder::build() {
    std::vector<AtlasTexture> unsorted_result;
    std::vector<uint32_t> indices;
    indices.reserve(this->textures.size());
    for (uint32_t i = 0; i < this->textures.size(); ++i) indices.push_back(i);
    std::stable_sort(indices.begin(), indices.end(),
                     [this](const uint32_t& a, const uint32_t& b) {
                         const RawTexture& tex_a = this->textures[a];
                         const RawTexture& tex_b = this->textures[b];
                         return std::max(tex_a.width, tex_a.height) >
                                std::max(tex_b.width, tex_b.height);
                     });

    std::unique_ptr<Node> root =
        std::make_unique<Node>(0, 0, this->textures[indices[0]].width,
                               this->textures[indices[0]].height);
    for (const uint32_t& i : indices) {
        const RawTexture& tex = this->textures[i];

        Node* node = root->find_node(tex.width, tex.height);
        Node* result_node;
        if (node) {
            result_node = node->split_node(tex.width, tex.height);
        } else {
            result_node = Node::grow_node(root, tex.width, tex.height);
        }
        unsorted_result.emplace_back(
            AtlasTexture{(uint16_t)result_node->x, (uint16_t)result_node->y,
                         (uint16_t)tex.width, (uint16_t)tex.height});
    }
    std::vector<AtlasTexture> result;
    for (const uint32_t& i : indices)
        result.push_back(unsorted_result[indices[i]]);
    std::vector<char> atlas_data;
    uint32_t atlas_width = root->w;
    uint32_t atlas_height = root->h;
    uint32_t atlas_px_width = root->w * 4;
    atlas_data.resize(atlas_width * atlas_height * 4);
    for (size_t i = 0; i < result.size(); i++) {
        const AtlasTexture& pos = result[i];
        const std::vector<char>& data = this->textures[i].data;
        const uint32_t& width = this->textures[i].width;
        const uint32_t& height = this->textures[i].height;
        for (uint32_t row = 0; row < height; row++) {
            char* dest = atlas_data.data() +
                         ((row + pos.y) * atlas_px_width + pos.x * 4);
            const char* src = data.data() + (row * width * 4);
            std::memcpy(dest, src, width * 4);
        }
    }
    return Resource(result, atlas_width, atlas_height, atlas_data, this->other);
}

Resource::Resource(std::vector<AtlasTexture> textures, uint32_t width,
                   uint32_t height, std::vector<char> data,
                   std::vector<std::vector<char>> other)
    : textures(textures),
      atlas_height(height),
      atlas_width(width),
      atlas_data(data),
      other(other) {}

Atlas Resource::get_atlas() {
    return Atlas{this->atlas_width, this->atlas_height, this->atlas_data};
}

const AtlasTexture& Resource::get_texture(uint32_t index) {
    return this->textures.at(index);
}

const std::vector<char>& Resource::get_other(uint32_t index) {
    return this->other.at(index);
}