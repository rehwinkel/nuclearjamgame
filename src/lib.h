#pragma once

#include "core.h"
#include "util.h"

const static float PIXELS_PER_UNIT = 32;

class SpriteComponent : public Component {
    AtlasTexture texture;
    uint32_t level;

   public:
    SpriteComponent(Game& game, std::weak_ptr<Entity> entity,
                    AtlasTexture texture, uint32_t level);
    virtual ~SpriteComponent();
    virtual void update(double delta);
    virtual void render(Renderer& renderer, uint8_t pass);
};

class ColorSpriteComponent : public SpriteComponent {
    AtlasTexture texture;
    uint32_t level;
    float red, green, blue;

   public:
    ColorSpriteComponent(Game& game, std::weak_ptr<Entity> entity,
                         AtlasTexture texture, uint32_t level, float r, float g,
                         float b);
    virtual ~ColorSpriteComponent();
    virtual void update(double delta);
    virtual void render(Renderer& renderer, uint8_t pass);
};