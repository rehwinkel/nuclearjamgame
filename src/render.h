#pragma once

#include "window.h"
#include "shader.h"
#include "resource.h"

class Renderer {
    Window m_window;
    uint8_t m_passes;
    uint16_t m_atlas_width, m_atlas_height;
    uint32_t atlas_texture;
    float m_camera_x, m_camera_y, m_camera_scale;
    WorldShader m_world_shader;

   public:
    Renderer(Window window, uint16_t width, uint16_t height, const char *data);
    void draw_sprite(AtlasTexture tex, float x, float y, uint32_t z_level,
                     float rotation, float ppu_x, float ppu_y);
    void pre_render();
    void post_render();
    bool keep_open();
    void set_color(float r, float g, float b);
    void set_color(float r, float g, float b, float a);
    float &camera_x();
    float &camera_y();
    float &camera_scale();
    Window &get_window();
    WorldShader &world_shader();
};