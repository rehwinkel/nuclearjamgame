#include <glad/glad.h>
#include <string>
#include <mat4x4.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include "core.h"

Renderer::Renderer(Window window, uint16_t width, uint16_t height,
                   const char* data)
    : m_window(window),
      m_atlas_width(width),
      m_atlas_height(height),
      m_camera_x(0),
      m_camera_y(0),
      m_camera_scale(1),
      m_world_shader(),
      m_gui_shader() {
    this->m_world_shader.load_uniforms();
    this->m_gui_shader.load_uniforms();

    GLuint atlas_tex;
    glGenTextures(1, &atlas_tex);
    glBindTexture(GL_TEXTURE_2D, atlas_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnableVertexAttribArray(0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->atlas_texture);
    this->m_world_shader.start();

    float ar = this->m_window.width() / this->m_window.height();
    glm::mat4 orthographic = glm::ortho(-ar, +ar, -1.0f, 1.0f, 0.0f, 1000.0f);
    glUniformMatrix4fv(this->m_world_shader.get_uni(WorldUniform::W_PROJ_MAT),
                       1, false, glm::value_ptr(orthographic));
    this->set_color(1.0, 1.0, 1.0);
    this->m_world_shader.stop();
}

float& Renderer::camera_x() { return this->m_camera_x; }

float& Renderer::camera_y() { return this->m_camera_y; }

float& Renderer::camera_scale() { return this->m_camera_scale; }

void Renderer::pre_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::post_render() { this->m_window.refresh(); }

// ppu = pixels per unit
void Renderer::draw_sprite(AtlasTexture tex, Point2f pos, uint32_t z_level,
                           float rotation, float ppu_x, float ppu_y) {
    if (z_level < 1 || z_level > 999) panic("invalid z level for sprite");
    uint32_t uv_mapping = this->m_world_shader.get_uni(WorldUniform::W_UV);
    uint32_t model = this->m_world_shader.get_uni(WorldUniform::W_MODEL_MAT);
    glUniform4f(uv_mapping, (float)tex.x / (float)this->m_atlas_width,
                (float)tex.y / (float)this->m_atlas_height,
                (float)(tex.x + tex.width) / (float)this->m_atlas_width,
                (float)(tex.y + tex.height) / (float)this->m_atlas_height);

    glm::mat4 model_mat(1.0f);
    model_mat = glm::scale(model_mat, glm::vec3(2.0f / this->m_camera_scale));
    model_mat = glm::translate(
        model_mat, glm::vec3(pos.x - this->m_camera_x, pos.y - this->m_camera_y,
                             (float)z_level - 1000.0f));
    model_mat = glm::rotate(model_mat, glm::radians(-rotation),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    float x_scale = (float)tex.width / (float)ppu_x * 0.5f;
    float y_scale = (float)tex.height / (float)ppu_y * 0.5f;
    model_mat = glm::scale(model_mat, glm::vec3(x_scale, y_scale, 1.0f));
    glUniformMatrix4fv(model, 1, false, glm::value_ptr(model_mat));
    this->set_color(1.0, 1.0, 1.0, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ppu = pixels per unit
void Renderer::draw_gui_texture(AtlasTexture tex, Point2i pos1, Point2i pos2) {
    uint32_t uv_mapping = this->m_gui_shader.get_uni(GuiUniform::G_UV);
    uint32_t model = this->m_gui_shader.get_uni(GuiUniform::G_MODEL_MAT);
    glUniform4f(uv_mapping, (float)tex.x / (float)this->m_atlas_width,
                (float)tex.y / (float)this->m_atlas_height,
                (float)(tex.x + tex.width) / (float)this->m_atlas_width,
                (float)(tex.y + tex.height) / (float)this->m_atlas_height);

    glm::mat4 model_mat(1.0f);
    float p1x = (float)pos1.x / this->m_window.width();
    float p1y = (float)pos1.y / this->m_window.height();
    float p2x = (float)pos2.x / this->m_window.width();
    float p2y = (float)pos2.y / this->m_window.height();
    float scale_x = p2x - p1x;
    float scale_y = p2y - p1y;
    model_mat = glm::translate(model_mat, glm::vec3(-1, +1, 0.0f));
    model_mat = glm::scale(model_mat, glm::vec3(scale_x, scale_y, 1.0f));
    model_mat = glm::translate(model_mat, glm::vec3(1, -1, 0));
    model_mat = glm::translate(
        model_mat, glm::vec3(p1x / scale_x * 2, -p1y / scale_y * 2, 0));
    glUniformMatrix4fv(model, 1, false, glm::value_ptr(model_mat));
    glUniform4f(this->m_gui_shader.get_uni(GuiUniform::G_COLOR_MUL), 0, 0, 0,
                0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ppu = pixels per unit
void Renderer::draw_gui_color(float r, float g, float b, float a, Point2i pos1,
                              Point2i pos2) {
    uint32_t model = this->m_gui_shader.get_uni(GuiUniform::G_MODEL_MAT);

    glm::mat4 model_mat(1.0f);
    float p1x = (float)pos1.x / this->m_window.width();
    float p1y = (float)pos1.y / this->m_window.height();
    float p2x = (float)pos2.x / this->m_window.width();
    float p2y = (float)pos2.y / this->m_window.height();
    float scale_x = p2x - p1x;
    float scale_y = p2y - p1y;
    model_mat = glm::translate(model_mat, glm::vec3(-1, +1, 0.0f));
    model_mat = glm::scale(model_mat, glm::vec3(scale_x, scale_y, 1.0f));
    model_mat = glm::translate(model_mat, glm::vec3(1, -1, 0));
    model_mat = glm::translate(
        model_mat, glm::vec3(p1x / scale_x * 2, -p1y / scale_y * 2, 0));
    glUniformMatrix4fv(model, 1, false, glm::value_ptr(model_mat));
    glUniform4f(this->m_gui_shader.get_uni(GuiUniform::G_COLOR_MUL), r, g, b,
                a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Renderer::keep_open() { return this->m_window.keep_open(); }

Window& Renderer::get_window() { return this->m_window; }

void Renderer::set_color(float r, float g, float b) {
    this->set_color(r, g, b, 1.0);
}

void Renderer::set_color(float r, float g, float b, float a) {
    glUniform4f(this->m_world_shader.get_uni(WorldUniform::W_COLOR_MUL), r, g,
                b, a);
}

WorldShader& Renderer::world_shader() { return this->m_world_shader; }

GuiShader& Renderer::gui_shader() { return this->m_gui_shader; }

void Renderer::update(double& last_time,
                      std::vector<std::weak_ptr<Entity>>& entities) {
    this->pre_render();

    glEnable(GL_DEPTH_TEST);

    this->m_world_shader.start();
    for (std::weak_ptr<Entity>& entity_weak : entities) {
        if (!entity_weak.expired()) {
            std::shared_ptr<Entity> entity = entity_weak.lock();
            entity->render(*this, 0);
        }
    }
    this->m_world_shader.stop();

    glDisable(GL_DEPTH_TEST);

    this->m_gui_shader.start();
    for (std::weak_ptr<Entity>& entity_weak : entities) {
        if (!entity_weak.expired()) {
            std::shared_ptr<Entity> entity = entity_weak.lock();
            entity->render(*this, 1);
        }
    }
    this->gui_shader().stop();

    last_time = glfwGetTime();
    this->post_render();
}