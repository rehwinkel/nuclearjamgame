#include "shader.h"

#include <string>
#include <glad/glad.h>

#include "world_shader.h"
#include "gui_shader.h"

void check_shader(GLuint shader) {
    GLint compile_result = GL_FALSE;
    int log_length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        std::vector<char> message(log_length + 1);
        glGetShaderInfoLog(shader, log_length, nullptr, message.data());
        std::string msg("failed to compile shader: ");
        msg += message.data();
        panic(msg.c_str());
    }
    if (!compile_result) {
        panic("failed to compile shader");
    }
}

Shader::Shader(const char* vert_src, const char* frag_src,
               uint32_t max_uniforms)
    : uniforms(max_uniforms) {
    this->vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vert_shader, 1, &vert_src, nullptr);
    glCompileShader(this->vert_shader);
    check_shader(this->vert_shader);

    this->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->frag_shader, 1, &frag_src, nullptr);
    glCompileShader(this->frag_shader);
    check_shader(this->frag_shader);

    this->program = glCreateProgram();
    glAttachShader(this->program, this->vert_shader);
    glAttachShader(this->program, this->frag_shader);
    glLinkProgram(this->program);
}

uint32_t Shader::get_uni(uint32_t id) { return this->uniforms.at(id); }

void Shader::start() { glUseProgram(this->program); }

void Shader::stop() { glUseProgram(0); }

void Shader::load_uniforms() {}

WorldShader::WorldShader()
    : Shader(world_shader_vert_source, world_shader_frag_source,
             WorldUniform::W_MAX_ELEMENT) {}

void WorldShader::load_uniforms() {
    this->uniforms[WorldUniform::W_UV] =
        glGetUniformLocation(program, "uv_mapping");
    this->uniforms[WorldUniform::W_PROJ_MAT] =
        glGetUniformLocation(program, "projection");
    this->uniforms[WorldUniform::W_MODEL_MAT] =
        glGetUniformLocation(program, "model");
    this->uniforms[WorldUniform::W_COLOR_MUL] =
        glGetUniformLocation(program, "color_mult");
}

GuiShader::GuiShader()
    : Shader(gui_shader_vert_source, gui_shader_frag_source,
             GuiUniform::G_MAX_ELEMENT) {}

void GuiShader::load_uniforms() {
    this->uniforms[GuiUniform::G_UV] =
        glGetUniformLocation(program, "uv_mapping");
    this->uniforms[GuiUniform::G_MODEL_MAT] =
        glGetUniformLocation(program, "model");
    this->uniforms[GuiUniform::G_COLOR_MUL] =
        glGetUniformLocation(program, "color_mult");
}