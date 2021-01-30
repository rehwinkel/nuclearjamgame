#pragma once

#include "util.h"

class Window {
    void* m_window;
    uint16_t m_width, m_height;

   public:
    Window(uint16_t width, uint16_t height, const char* title);
    float width();
    float height();
    void refresh();
    bool keep_open();
};