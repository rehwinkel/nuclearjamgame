#include <resource.h>
#include <core.h>
#include <util.h>

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

const static float PIXELS_PER_UNIT = 32;

class SpriteComponent : public Component {
    AtlasTexture texture;

   public:
    SpriteComponent(Game& game, std::weak_ptr<Entity> entity,
                    AtlasTexture texture)
        : Component(game, entity), texture(texture) {}
    virtual ~SpriteComponent() {}
    virtual void update(double delta) {}
    virtual void render(Renderer& renderer) {
        std::shared_ptr<Entity> this_entity = this->entity.lock();
        renderer.draw_sprite(this->texture, this_entity->x, this_entity->y, 1,
                             this_entity->rotation,
                             this_entity->size_x * PIXELS_PER_UNIT,
                             this_entity->size_y * PIXELS_PER_UNIT);
        renderer.draw_sprite(this->texture, this_entity->x + 1, this_entity->y,
                             1, this_entity->rotation,
                             this_entity->size_x * PIXELS_PER_UNIT,
                             this_entity->size_y * PIXELS_PER_UNIT);
        if (this->game.is_key_down(GLFW_KEY_SPACE))
            std::cout << "SPaes" << std::endl;
    }
};

int main(int argc, char const* argv[]) {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(1280, 720, "Window"), atlas.width, atlas.height,
                       atlas.data.data()));
    AtlasTexture gimp_tex = res.get_texture(1);

    auto ptr = game.add_entity<Entity>();
    ptr->add_component<SpriteComponent>(gimp_tex);
    ptr->add_child<Entity>();
    game.renderer().camera_scale() = 8;
    game.renderer().camera_x() = 0;

    game.run();
    return 0;
}