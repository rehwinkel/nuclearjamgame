#include "resource.h"

#include <string>
#include <fstream>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

int main(int argc, char const* argv[]) {
    if (argc < 3) {
        std::cout << "usage: " << argv[0] << " NAME FILE+" << std::endl;
    }

    ResourceBuilder builder;
    std::string out_name(argv[1]);

    std::ofstream map_file(out_name + ".map");
    for (int i = 2; i < argc; i++) {
        std::string path(argv[i]);
        uint32_t id;
        std::string ending(path.c_str() + path.size() - 4);
        if (ending == ".png" || ending == ".jpg") {
            id = builder.add_texture(path.c_str());
        } else {
            id = builder.add_other(path.c_str());
        }
        map_file << id << ": " << path << std::endl;
    }

    Resource r = builder.build();
    stbi_write_png((out_name + ".png").c_str(), r.get_atlas().width,
                   r.get_atlas().height, 4, r.get_atlas().data.data(), 0);
    std::vector<char> data = r.to_buffer();
    std::ofstream c_file(out_name + ".c");

    c_file << "const unsigned long long " << out_name << "_size"
           << " = " << data.size() << ";" << std::endl;
    c_file << "const unsigned char " << out_name << "_data[" << data.size()
           << "] = {";
    for (uint32_t i = 0; i < data.size(); ++i) {
        c_file << (int)(unsigned char)data[i] << ", ";
        if (i % 30 == 0) c_file << std::endl;
    }
    c_file << "};" << std::endl;

    return 0;
}
