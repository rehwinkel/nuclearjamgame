#include <resource.h>
#include <core.h>
#include <util.h>

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

const static float PIXELS_PER_UNIT = 32;

class SpriteComponent : public Component {
    AtlasTexture texture;
    uint32_t level;

   public:
    SpriteComponent(Game& game, std::weak_ptr<Entity> entity,
                    AtlasTexture texture, uint32_t level)
        : Component(game, entity), texture(texture), level(level) {}
    virtual ~SpriteComponent() {}
    virtual void update(double delta) {
        std::shared_ptr<Entity> this_entity = this->entity.lock();
        this_entity->x += delta;
    }
    virtual void render(Renderer& renderer) {
        std::shared_ptr<Entity> this_entity = this->entity.lock();
        renderer.draw_sprite(this->texture, this_entity->x, this_entity->y,
                             level, this_entity->rotation,
                             this_entity->size_x * PIXELS_PER_UNIT,
                             this_entity->size_y * PIXELS_PER_UNIT);
    }
};

int main() {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(1280, 720, "Window"), atlas.width, atlas.height,
                       atlas.data.data()));
    AtlasTexture red = res.get_texture(7);

    auto ptr2 =
        game.add_entity<Entity>(std::weak_ptr<Entity>{}, 0.5, 0.5, 0, 1, 1);
    ptr2->add_component<SpriteComponent>(red, 1);

    game.renderer().camera_scale() = 8;
    game.renderer().camera_x() = 0;

    game.run();
    return 0;
}