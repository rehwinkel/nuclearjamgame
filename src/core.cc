#include "core.h"

Game::Game(Renderer renderer) : m_renderer(renderer) {}

void Game::run() {
    while (this->m_renderer.keep_open()) {
        // TODO: update game logic
        this->m_renderer.refresh();
    }
}