#include "core.h"

Component::Component(std::weak_ptr<Entity> entity) : entity(entity) {}

Component::~Component() {}

Entity::Entity(Game& game, std::weak_ptr<Entity> parent, float x, float y,
               float rotation, float size_x, float size_y)
    : game(game),
      parent{},
      x(x),
      y(y),
      rotation(rotation),
      size_x(size_x),
      size_y(size_y) {}

Entity::Entity(Game& game, std::weak_ptr<Entity> parent)
    : Entity(game, parent, 0, 0, 0, 1, 1) {}

Entity::Entity(Game& game) : Entity(game, std::weak_ptr<Entity>{}) {}

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

Game::Game(Renderer renderer) : m_renderer(renderer) {}

void Game::run() {
    while (this->m_renderer.keep_open()) {
        double delta = 0;
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
        this->m_renderer.post_render();
    }
}

std::vector<std::weak_ptr<Entity>>& Game::get_entities() {
    return this->entities;
}

Renderer& Game::renderer() { return this->m_renderer; }
