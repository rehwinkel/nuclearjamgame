#pragma once

#include "window.h"
#include "resource.h"

enum Uniform {
    UV,
    MAX_ELEMENT,
};

class Renderer {
    Window m_window;
    uint16_t m_atlas_width, m_atlas_height;
    uint32_t atlas_texture;
    uint32_t uniforms[Uniform::MAX_ELEMENT];

   public:
    Renderer(Window window, uint16_t width, uint16_t height, const char* data);
    void draw_sprite(AtlasTexture texture);
    void pre_render();
    void post_render();
    bool keep_open();
};