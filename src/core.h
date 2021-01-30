#pragma once

#include "render.h"

#include <vector>
#include <memory>

class Entity;

class Component {
    std::weak_ptr<Entity> entity;

   public:
    Component(std::weak_ptr<Entity> entity);
    virtual ~Component();
    virtual void update(double delta) = 0;
    virtual void render(Renderer& renderer) = 0;
};

class Game;

class Entity : public std::enable_shared_from_this<Entity> {
    Game& game;
    std::vector<std::unique_ptr<Component>> components;

   public:
    Entity(Game& game);
    virtual ~Entity();
    template <class T, class... Args>
    void add_component(Args... v) {
        std::unique_ptr<T> comp = std::make_unique<T>(
            std::weak_ptr<Entity>(shared_from_this()), v...);
        this->components.emplace_back(std::move(comp));
    }
    void update(double delta);
    void render(Renderer& renderer);
};

class Game {
    Renderer m_renderer;
    std::vector<std::weak_ptr<Entity>> entities;

   public:
    Game(Renderer renderer);
    template <class T, class... Args>
    std::shared_ptr<T> add_entity(Args... v) {
        std::shared_ptr<T> entity = std::make_shared<T>(*this, v...);
        entities.emplace_back(std::weak_ptr<T>(entity));
        return entity;
    }
    void run();
};