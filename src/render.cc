#include "render.h"

#include <glad/glad.h>
#include <string>
#include <mat4x4.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

const char* vert_source = R""""(

#version 130

in vec3 position;

out vec2 uv;

uniform mat4 projection;
uniform mat4 model;

void main(void) {
    gl_Position = projection * model * vec4(position, 1.0);
    uv = (position.xy + 1.0) / 2.0;
}

)"""";

const char* frag_source = R""""(

#version 130

in vec2 uv;

out vec4 color;

uniform sampler2D albedo;

uniform vec4 uv_mapping;

float map(float value, float min1, float max1, float min2, float max2) {
    float perc = (value - min1) / (max1 - min1);
    return perc * (max2 - min2) + min2;
}

void main(void) {
    float u = map(uv.x, 0, 1, uv_mapping.x, uv_mapping.z);
    float v = map(uv.y, 1, 0, uv_mapping.y, uv_mapping.w);
    vec2 final_uv = vec2(u, v);
    final_uv = vec2(final_uv.x, final_uv.y);
    color = texture2D(albedo, final_uv);
}

)"""";

void check_shader(GLuint shader) {
    GLint compile_result = GL_FALSE;
    int log_length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > 0) {
        char message[log_length + 1];
        glGetShaderInfoLog(shader, log_length, nullptr, message);
        std::string msg("failed to compile shader: ");
        msg += message;
        panic(msg.c_str());
    }
    if (!compile_result) {
        panic("failed to compile shader");
    }
}

Renderer::Renderer(Window window, uint16_t width, uint16_t height,
                   const char* data)
    : m_window(window), m_atlas_width(width), m_atlas_height(height) {
    GLuint atlas_tex;
    glGenTextures(1, &atlas_tex);
    glBindTexture(GL_TEXTURE_2D, atlas_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    this->atlas_texture = atlas_tex;

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertices[] = {
        1,  1,  0,  // top right
        1,  -1, 0,  // bottom right
        -1, -1, 0,  // bottom left

        1,  1,  0,  // top right
        -1, -1, 0,  // bottom left
        -1, 1,  0   // top left
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

    glClearColor(1.0, 0.0, 1.0, 1.0);
    glEnableVertexAttribArray(0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_source, nullptr);
    glCompileShader(frag_shader);
    check_shader(frag_shader);

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_source, nullptr);
    glCompileShader(vert_shader);
    check_shader(vert_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->atlas_texture);
    this->uniforms[Uniform::UV] = glGetUniformLocation(program, "uv_mapping");
    this->uniforms[Uniform::PROJ_MAT] =
        glGetUniformLocation(program, "projection");
    this->uniforms[Uniform::MODEL_MAT] = glGetUniformLocation(program, "model");
    float ar = this->m_window.width() / this->m_window.height();
    glm::mat4 orthographic = glm::ortho(-ar, +ar, -1.0f, 1.0f, 0.0f, 1000.0f);
    glUniformMatrix4fv(this->uniforms[Uniform::PROJ_MAT], 1, false,
                       glm::value_ptr(orthographic));
}

void Renderer::pre_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::post_render() { this->m_window.refresh(); }

// ppu = pixels per unit
void Renderer::draw_sprite(AtlasTexture tex, float x, float y, uint32_t z_level,
                           float rotation, float ppu_x, float ppu_y) {
    if (z_level < 1 || z_level > 999) panic("invalid z level for sprite");
    uint32_t uv_mapping = this->uniforms[Uniform::UV];
    uint32_t model = this->uniforms[Uniform::MODEL_MAT];
    glUniform4f(uv_mapping, (float)tex.x / (float)this->m_atlas_width,
                (float)tex.y / (float)this->m_atlas_height,
                (float)(tex.x + tex.width) / (float)this->m_atlas_width,
                (float)(tex.y + tex.height) / (float)this->m_atlas_height);

    glm::mat4 model_mat(1.0f);
    model_mat = glm::translate(model_mat, glm::vec3(x, y, z_level - 1000.0f));
    model_mat = glm::rotate(model_mat, glm::radians(-rotation),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    float x_scale = (float)tex.width / (float)ppu_x;
    float y_scale = (float)tex.height / (float)ppu_y;
    model_mat = glm::scale(model_mat, glm::vec3(x_scale, y_scale, 1.0f));
    glUniformMatrix4fv(model, 1, false, glm::value_ptr(model_mat));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Renderer::keep_open() { return this->m_window.keep_open(); }