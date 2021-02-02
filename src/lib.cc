#include "lib.h"

SpriteComponent::SpriteComponent(Game& game, std::weak_ptr<Entity> entity,
                                 AtlasTexture texture, uint32_t level)
    : Component(game, entity), texture(texture), level(level) {}

SpriteComponent::~SpriteComponent() {}

void SpriteComponent::update(double delta) { (void)delta; }

void SpriteComponent::render(Renderer& renderer, uint8_t pass) {
    (void)pass;
    std::shared_ptr<Entity> this_entity = this->entity.lock();
    renderer.draw_sprite(this->texture, Point2f{this_entity->x, this_entity->y},
                         level, this_entity->rotation,
                         this_entity->size_x * PIXELS_PER_UNIT,
                         this_entity->size_y * PIXELS_PER_UNIT);
}

ColorSpriteComponent::ColorSpriteComponent(Game& game,
                                           std::weak_ptr<Entity> entity,
                                           AtlasTexture texture, uint32_t level,
                                           float r, float g, float b)
    : SpriteComponent(game, entity, texture, level),
      red(r),
      green(g),
      blue(b) {}

ColorSpriteComponent::~ColorSpriteComponent() {}

void ColorSpriteComponent::update(double delta) { (void)delta; }

void ColorSpriteComponent::render(Renderer& renderer, uint8_t pass) {
    renderer.set_color(this->red, this->green, this->blue);
    SpriteComponent::render(renderer, pass);
    renderer.set_color(1.0, 1.0, 1.0);
}