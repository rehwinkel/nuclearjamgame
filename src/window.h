#pragma once

#include "util.h"

class Window {
    void* m_window;

   public:
    Window(uint16_t width, uint16_t height, const char* title);
    void refresh();
    bool keep_open();
};