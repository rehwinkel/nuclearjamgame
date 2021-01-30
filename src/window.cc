#include <glad/glad.h>

#include "core.h"
#include "window.h"

Window::Window(uint16_t width, uint16_t height, const char* title)
    : m_width(width), m_height(height) {
    if (glfwInit() != GLFW_TRUE) panic("failed to initialize glfw");
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    this->m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(this->m_window);
    glfwSwapInterval(1);
    gladLoadGLLoader(
        [](const char* func) { return (void*)glfwGetProcAddress(func); });
}

void Window::setup_callbacks(Game& game) {
    glfwSetWindowUserPointer(this->m_window, &game);
    glfwSetKeyCallback(this->m_window, [](GLFWwindow* window, int key,
                                          int scancode, int action, int mods) {
        Game& game = *(Game*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                game._key_states[key] = true;
                break;
            default:
                game._key_states[key] = false;
                break;
        }
    });
    glfwSetMouseButtonCallback(
        this->m_window,
        [](GLFWwindow* window, int button, int action, int mods) {
            Game& game = *(Game*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS:
                case GLFW_REPEAT:
                    game._mb_states[button] = true;
                    break;
                default:
                    game._mb_states[button] = false;
                    break;
            }
        });
    glfwSetCursorPosCallback(
        this->m_window, [](GLFWwindow* window, double x, double y) {
            Game& game = *(Game*)glfwGetWindowUserPointer(window);
            game._mouse_x = x;
            game._mouse_y = y;
        });
    glfwSetScrollCallback(
        this->m_window, [](GLFWwindow* window, double x, double y) {
            Game& game = *(Game*)glfwGetWindowUserPointer(window);
            game._scroll_x += x;
            game._scroll_y += y;
        });
}

bool Window::keep_open() { return !glfwWindowShouldClose(this->m_window); }

void Window::refresh() {
    glfwPollEvents();
    glfwSwapBuffers(this->m_window);
}

float Window::width() { return (float)this->m_width; }

float Window::height() { return (float)this->m_height; }