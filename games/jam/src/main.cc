#include <resource.h>
#include <lib.h>

#include "util.h"
#include "buildings.h"

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

class BuildingEntity : public Entity {
    Building& building;
    int16_t grid_x;
    int16_t grid_y;

   public:
    BuildingEntity(Game& game, Building& building, int16_t x, int16_t y)
        : Entity(game), building(building), grid_x(x), grid_y(y) {}
    virtual void init() {
        this->x = (float)grid_x;
        this->y = (float)grid_y;
        double hue = (double)rand() / (double)RAND_MAX;
        rgb col = hsv2rgb(hsv{hue * 360.0, 0.2, 1});
        this->add_component<ColorSpriteComponent>(this->building.texture, 1,
                                                  col.r, col.g, col.b);
        if (building.texture_overlay.width > 0 &&
            building.texture_overlay.height > 0) {
            this->add_component<SpriteComponent>(this->building.texture_overlay,
                                                 2);
        } else {
            this->add_component<SpriteComponent>(this->building.texture, 1);
        }
    }
    virtual ~BuildingEntity() {}
};

int main() {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(1280, 720, "InfraGame"), atlas.width,
                       atlas.height, atlas.data.data()));

    AtlasTexture house_bg = res.get_texture(4);
    AtlasTexture house_fg = res.get_texture(3);
    Building house{EnergyType::CONSUMER, house_bg, house_fg, 0, 0, 1, 1};

    auto first_house1 = game.add_entity<BuildingEntity>(house, 0, 0);
    auto first_house2 = game.add_entity<BuildingEntity>(house, 0, 1);
    auto first_house3 = game.add_entity<BuildingEntity>(house, 1, 0);
    auto first_house4 = game.add_entity<BuildingEntity>(house, 1, 1);
    auto first_house5 = game.add_entity<BuildingEntity>(house, 0, -1);
    auto first_house6 = game.add_entity<BuildingEntity>(house, -1, 0);
    auto first_house7 = game.add_entity<BuildingEntity>(house, -1, -1);
    auto first_house8 = game.add_entity<BuildingEntity>(house, 1, -1);
    auto first_house9 = game.add_entity<BuildingEntity>(house, -1, 1);

    game.renderer().camera_scale() = 8;
    game.renderer().camera_x() = 0;

    game.run();
    return 0;
}