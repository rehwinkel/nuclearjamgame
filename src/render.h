#pragma once

#include "window.h"
#include "resource.h"

enum Uniform {
    UV,
    PROJ_MAT,
    MODEL_MAT,
    MAX_ELEMENT,
};

class Renderer {
    Window m_window;
    uint16_t m_atlas_width, m_atlas_height;
    uint32_t atlas_texture;
    uint32_t uniforms[Uniform::MAX_ELEMENT];
    float m_camera_x, m_camera_y, m_camera_scale;

   public:
    Renderer(Window window, uint16_t width, uint16_t height, const char *data);
    void draw_sprite(AtlasTexture tex, float x, float y, uint32_t z_level,
                     float rotation, float ppu_x, float ppu_y);
    void pre_render();
    void post_render();
    bool keep_open();
    float &camera_x();
    float &camera_y();
    float &camera_scale();
    Window &get_window();
};