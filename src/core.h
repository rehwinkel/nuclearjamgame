#pragma once

#include "render.h"

class Game {
    Renderer m_renderer;

   public:
    Game(Renderer renderer);
    void run();
};