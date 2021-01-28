#pragma once

#include "window.h"

class Renderer {
    Window m_window;

   public:
    Renderer(Window window);
    void refresh();
    bool keep_open();
};