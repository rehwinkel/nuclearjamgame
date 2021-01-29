#include "render.h"

#include <glad/glad.h>
#include <string>

const char* vert_source = R""""(

#version 130

in vec3 position;

out vec2 uv;

void main(void) {
    gl_Position = vec4(position, 1.0);
    uv = (position.xy + 1.0) / 2.0;
}

)"""";

const char* frag_source = R""""(

#version 130

in vec2 uv;

out vec4 color;

uniform sampler2D albedo;

void main(void) {
    color = texture2D(albedo, vec2(uv.x, 1-uv.y));
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
}

void Renderer::refresh() {
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->atlas_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    this->m_window.refresh();
}

bool Renderer::keep_open() { return this->m_window.keep_open(); }