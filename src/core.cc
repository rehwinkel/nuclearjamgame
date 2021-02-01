#include "core.h"

Component::Component(Game& game, std::weak_ptr<Entity> entity)
    : game(game), entity(entity) {}

Component::~Component() {}

Entity::Entity(Game& game, std::weak_ptr<Entity> parent, float x, float y,
               float rotation, float size_x, float size_y)
    : game(game),
      parent(parent),
      x(x),
      y(y),
      rotation(rotation),
      size_x(size_x),
      size_y(size_y) {}

Entity::Entity(Game& game, std::weak_ptr<Entity> parent)
    : Entity(game, parent, 0, 0, 0, 1, 1) {}

Entity::Entity(Game& game) : Entity(game, std::weak_ptr<Entity>{}) {}

void Entity::init() {}

Entity::~Entity() {}

void Entity::update(double delta) {
    for (auto& component : this->components) {
        component->update(delta);
    }
}

void Entity::render(Renderer& renderer) {
    for (auto& component : this->components) {
        component->render(renderer);
    }
}

Game::Game(Renderer renderer)
    : m_renderer(renderer),
      _key_states{0},
      _mb_states{0},
      _mouse_x(0),
      _mouse_y(0),
      _scroll_x(0),
      _scroll_y(0) {
    this->m_renderer.get_window().setup_callbacks(*this);
}

void Game::run() {
    double last_time = glfwGetTime();
    while (this->m_renderer.keep_open()) {
        double delta = glfwGetTime() - last_time;
        for (std::weak_ptr<Entity>& entity_weak : this->entities) {
            if (!entity_weak.expired()) {
                std::shared_ptr<Entity> entity = entity_weak.lock();
                entity->update(delta);
            }
        }
        this->m_renderer.pre_render();
        for (std::weak_ptr<Entity>& entity_weak : this->entities) {
            if (!entity_weak.expired()) {
                std::shared_ptr<Entity> entity = entity_weak.lock();
                entity->render(this->m_renderer);
            }
        }
        last_time = glfwGetTime();
        this->m_renderer.post_render();
    }
}

std::vector<std::weak_ptr<Entity>>& Game::get_entities() {
    return this->entities;
}

bool Game::is_key_down(int key) {
    if (key >= GLFW_KEY_LAST || key < 0) panic("invalid keycode");
    return this->_key_states[key];
}

bool Game::is_button_down(int button) {
    if (button >= GLFW_KEY_LAST || button < 0) panic("invalid button");
    return this->_mb_states[button];
}
double Game::get_mouse_x() { return this->_mouse_x; }

double Game::get_mouse_y() { return this->_mouse_y; }

double Game::get_scroll_x() { return this->_scroll_x; }

double Game::get_scroll_y() { return this->_scroll_y; }

Renderer& Game::renderer() { return this->m_renderer; }
