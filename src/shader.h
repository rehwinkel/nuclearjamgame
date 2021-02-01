#include "util.h"

#include <vector>

class Shader {
   protected:
    uint32_t program;
    uint32_t vert_shader;
    uint32_t frag_shader;
    std::vector<uint32_t> uniforms;

   public:
    Shader(const char* vert_src, const char* frag_src, uint32_t max_uniforms);
    virtual void load_uniforms() = 0;
    uint32_t get_uni(uint32_t id);
    void start();
    void stop();
};

class WorldShader : public Shader {
   public:
    WorldShader();
    virtual void load_uniforms();
};

enum WorldUniform {
    UV,
    PROJ_MAT,
    MODEL_MAT,
    COLOR_MUL,
    MAX_ELEMENT,
};