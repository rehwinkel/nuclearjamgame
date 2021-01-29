#pragma once

#include "window.h"

class Renderer {
    Window m_window;
    uint16_t m_atlas_width, m_atlas_height;
    uint32_t atlas_texture;

   public:
    Renderer(Window window, uint16_t width, uint16_t height, const char* data);
    void refresh();
    bool keep_open();
};