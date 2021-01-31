#include <cstdint>
#include <lib.h>

enum EnergyType {
    PRODUCER,
    CONSUMER,
    TRANSPORTER,
};

struct Building {
    EnergyType type;
    AtlasTexture texture;
    AtlasTexture texture_overlay;
    uint16_t offset_x;
    uint16_t offset_y;
    uint16_t width;
    uint16_t height;
};
