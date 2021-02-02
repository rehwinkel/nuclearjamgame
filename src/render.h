#pragma once

#include "window.h"
#include "shader.h"
#include "resource.h"

#include <memory>

class Entity;

struct Point2f {
    float x, y;
};

struct Point2i {
    int x, y;
};

class Renderer {
    Window m_window;
    uint8_t m_passes;
    uint16_t m_atlas_width, m_atlas_height;
    uint32_t atlas_texture;
    float m_camera_x, m_camera_y, m_camera_scale;
    WorldShader m_world_shader;
    GuiShader m_gui_shader;

   public:
    Renderer(Window window, uint16_t width, uint16_t height, const char *data);
    void draw_sprite(AtlasTexture tex, Point2f pos, uint32_t z_level,
                     float rotation, float ppu_x, float ppu_y);
    void draw_gui_texture(AtlasTexture tex, Point2i pos1, Point2i pos2);
    void draw_gui_color(float r, float g, float b, float a, Point2i pos1,
                        Point2i pos2);
    void update(double &last_time,
                std::vector<std::weak_ptr<Entity>> &entities);
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
    GuiShader &gui_shader();
};