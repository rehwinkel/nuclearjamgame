#include <resource.h>
#include <core.h>
#include <util.h>

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

int main(int argc, char const *argv[]) {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Atlas atlas = res.get_atlas();
    Game game(Renderer(Window(640, 480, "Window"), atlas.width, atlas.height,
                       atlas.data.data()));
    AtlasTexture gimp_tex = res.get_texture(0);

    game.run();
    return 0;
}
