#include <resource.h>
#include <lib.h>
#include <cmath>

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
        this->add_component<ColorSpriteComponent>(this->building.texture, 2,
                                                  col.r, col.g, col.b);
        if (building.texture_overlay.width > 0 &&
            building.texture_overlay.height > 0) {
            this->add_component<SpriteComponent>(this->building.texture_overlay,
                                                 3);
        } else {
            this->add_component<SpriteComponent>(this->building.texture, 2);
        }
    }
    virtual ~BuildingEntity() {}
};

const static float GRID_CORRECTION_SCALE = 1.01f;

class GridBackgroundComponent : public Component {
    AtlasTexture texture;

   public:
    GridBackgroundComponent(Game& game, std::weak_ptr<Entity> entity,
                            AtlasTexture tex)
        : Component(game, entity), texture(tex) {}
    virtual ~GridBackgroundComponent() {}
    virtual void update(double delta) {}
    virtual void render(Renderer& renderer, uint8_t pass) {
        float cam_x = this->game.renderer().camera_x();
        float cam_y = this->game.renderer().camera_y();
        int16_t y_scale = (int16_t)this->game.renderer().camera_scale();
        int16_t half_y_scale = y_scale / 2 + 2;
        float aspect_ratio = this->game.renderer().get_window().width() /
                             this->game.renderer().get_window().height();
        int16_t x_scale =
            (int16_t)this->game.renderer().camera_scale() * aspect_ratio;
        int16_t half_x_scale = x_scale / 2 + 2;
        for (int16_t y = -half_y_scale + cam_y; y <= half_y_scale + cam_y;
             y++) {
            for (int16_t x = -half_x_scale + cam_x; x <= half_x_scale + cam_x;
                 x++) {
                renderer.draw_sprite(this->texture, x, y, 1, 0,
                                     PIXELS_PER_UNIT / GRID_CORRECTION_SCALE,
                                     PIXELS_PER_UNIT / GRID_CORRECTION_SCALE);
            }
        }
    }
};

const static float CAMERA_BORDER = 100;

class TopCameraComponent : public Component {
    float speed;
    float zoom_speed;
    float last_scroll_pos;

   public:
    TopCameraComponent(Game& game, std::weak_ptr<Entity> entity, float speed,
                       float zoom_speed)
        : Component(game, entity),
          speed(speed),
          last_scroll_pos(0),
          zoom_speed(zoom_speed) {}
    virtual ~TopCameraComponent() {}
    virtual void update(double delta) {
        float& cam_x = this->game.renderer().camera_x();
        float& cam_y = this->game.renderer().camera_y();
        float cam_step = delta * speed * this->game.renderer().camera_scale();
        if (this->game.is_key_down(GLFW_KEY_W) && cam_y < CAMERA_BORDER)
            cam_y += cam_step;
        if (this->game.is_key_down(GLFW_KEY_S) && cam_y > -CAMERA_BORDER)
            cam_y -= cam_step;
        if (this->game.is_key_down(GLFW_KEY_A) && cam_x > -CAMERA_BORDER)
            cam_x -= cam_step;
        if (this->game.is_key_down(GLFW_KEY_D) && cam_x < CAMERA_BORDER)
            cam_x += cam_step;
        double diff = last_scroll_pos - this->game.get_scroll_y();
        this->game.renderer().camera_scale() += diff * zoom_speed;
        if (this->game.renderer().camera_scale() < 2)
            this->game.renderer().camera_scale() = 2;
        if (this->game.renderer().camera_scale() > 16)
            this->game.renderer().camera_scale() = 16;

        last_scroll_pos = this->game.get_scroll_y();
    }
    virtual void render(Renderer& renderer, uint8_t pass) {}
};

class MousePickComponent : public Component {
    AtlasTexture texture;

   public:
    MousePickComponent(Game& game, std::weak_ptr<Entity> entity,
                       AtlasTexture selector)
        : Component(game, entity), texture(selector) {}
    virtual ~MousePickComponent() {}
    virtual void update(double delta) {}
    virtual void render(Renderer& renderer, uint8_t pass) {
        float aspect_ratio = this->game.renderer().get_window().width() /
                             this->game.renderer().get_window().height();
        float x = this->game.get_mouse_x() /
                      this->game.renderer().get_window().width() * 2 -
                  1;
        x *= (this->game.renderer().camera_scale() / 2) * aspect_ratio;
        float y = -(this->game.get_mouse_y() /
                        this->game.renderer().get_window().height() * 2 -
                    1);
        y *= this->game.renderer().camera_scale() / 2;
        x += this->game.renderer().camera_x();
        y += this->game.renderer().camera_y();
        renderer.draw_sprite(this->texture, std::round(x), std::round(y), 999,
                             0, PIXELS_PER_UNIT, PIXELS_PER_UNIT);
    }
};

int main() {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(1280, 720, "InfraGame"), atlas.width,
                       atlas.height, atlas.data.data()));

    AtlasTexture background = res.get_texture(1);
    AtlasTexture selector = res.get_texture(0);
    AtlasTexture house_bg = res.get_texture(3);
    AtlasTexture house_fg = res.get_texture(2);
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
    auto camera = game.add_entity<Entity>();
    camera->add_component<GridBackgroundComponent>(background);
    camera->add_component<TopCameraComponent>(1.0f, 1.0f);
    camera->add_component<MousePickComponent>(selector);

    game.renderer().camera_scale() = 8;
    game.renderer().camera_x() = 0;

    game.run();
    return 0;
}