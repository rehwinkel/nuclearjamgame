#include <resource.h>
#include <core.h>
#include <util.h>

extern "C" const unsigned long long jam_size;
extern "C" const char jam_data[];

int main(int argc, char const *argv[]) {
    Resource res = Resource::from_buffer(jam_data, jam_size);
    Game game(Renderer(Window(640, 480, "Window"))); 

    game.run();
    return 0;
}
