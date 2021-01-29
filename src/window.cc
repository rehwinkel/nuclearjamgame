#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window(uint16_t width, uint16_t height, const char* title) {
    if (glfwInit() != GLFW_TRUE) panic("failed to initialize glfw");
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    this->m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent((GLFWwindow*)this->m_window);
    glfwSwapInterval(1);
    gladLoadGLLoader(
        [](const char* func) { return (void*)glfwGetProcAddress(func); });
}

bool Window::keep_open() {
    return !glfwWindowShouldClose((GLFWwindow*)this->m_window);
}

void Window::refresh() {
    glfwPollEvents();
    glfwSwapBuffers((GLFWwindow*)this->m_window);
}