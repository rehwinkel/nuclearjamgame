#include <resource.h>
#include <core.h>
#include <util.h>

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

class SpriteComponent : public Component {
    AtlasTexture texture;

   public:
    SpriteComponent(std::weak_ptr<Entity> entity, AtlasTexture texture)
        : Component(entity), texture(texture) {}
    virtual ~SpriteComponent() {}
    virtual void update(double delta) {}
    virtual void render(Renderer& renderer) {
        renderer.draw_sprite(this->texture);
    }
};

int main(int argc, char const* argv[]) {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(640, 480, "Window"), atlas.width, atlas.height,
                       atlas.data.data()));
    AtlasTexture gimp_tex = res.get_texture(0);

    auto ptr = game.add_entity<Entity>();
    ptr->add_component<SpriteComponent>(gimp_tex);

    game.run();
    return 0;
}
