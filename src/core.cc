#include "core.h"

Component::Component(std::weak_ptr<Entity> entity) : entity(entity) {}

Component::~Component() {}

Entity::Entity(Game& game) : game(game) {}

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