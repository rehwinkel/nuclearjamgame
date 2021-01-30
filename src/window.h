#pragma once

#include "util.h"

#include <GLFW/glfw3.h>

class Game;

class Window {
    GLFWwindow* m_window;
    uint16_t m_width, m_height;

   public:
    Window(uint16_t width, uint16_t height, const char* title);
    void setup_callbacks(Game& game);
    float width();
    float height();
    void refresh();
    bool keep_open();
};