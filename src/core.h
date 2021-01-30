#pragma once

#include "render.h"

#include <vector>
#include <memory>

class Entity;

class Component {
   protected:
    std::weak_ptr<Entity> entity;

   public:
    Component(std::weak_ptr<Entity> entity);
    virtual ~Component();
    virtual void update(double delta) = 0;
    virtual void render(Renderer& renderer) = 0;
};

class Entity;

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
    std::vector<std::weak_ptr<Entity>>& get_entities();
    Renderer& renderer();
    void run();
};

class Entity : public std::enable_shared_from_this<Entity> {
   protected:
    Game& game;
    std::weak_ptr<Entity> parent;
    std::vector<std::unique_ptr<Component>> components;
    std::vector<std::shared_ptr<Entity>> children;

   public:
    float x, y, rotation, size_x, size_y;

    Entity(Game& game, std::weak_ptr<Entity> parent, float x, float y,
           float rotation, float size_x, float size_y);
    Entity(Game& game, std::weak_ptr<Entity> parent);
    Entity(Game& game);
    virtual ~Entity();
    template <class T, class... Args>
    void add_component(Args... v) {
        std::unique_ptr<T> comp = std::make_unique<T>(
            std::weak_ptr<Entity>(shared_from_this()), v...);
        this->components.emplace_back(std::move(comp));
    }
    template <class T, class... Args>
    std::shared_ptr<T> add_child(Args... v) {
        std::shared_ptr<T> entity = std::make_shared<T>(
            this->game, std::weak_ptr<Entity>(shared_from_this()), v...);
        this->game.get_entities().emplace_back(std::weak_ptr<T>(entity));
        this->children.push_back(entity);
        return entity;
    }
    void update(double delta);
    void render(Renderer& renderer);
};