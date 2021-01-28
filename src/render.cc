#include "render.h"

#include <glad/glad.h>

Renderer::Renderer(Window window) : m_window(window) {}

void Renderer::refresh() {
    glClear(GL_COLOR_BUFFER_BIT);
    this->m_window.refresh();
}

bool Renderer::keep_open() { return this->m_window.keep_open(); }