#pragma once

const char* world_shader_vert_source = R""""(

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

const char* world_shader_frag_source = R""""(

#version 130

in vec2 uv;

out vec4 color;

uniform sampler2D albedo;

uniform vec4 uv_mapping;
uniform vec4 color_mult;

float map(float value, float min1, float max1, float min2, float max2) {
    float perc = (value - min1) / (max1 - min1);
    return perc * (max2 - min2) + min2;
}

void main(void) {
    float u = map(uv.x, 0, 1, uv_mapping.x, uv_mapping.z);
    float v = map(uv.y, 1, 0, uv_mapping.y, uv_mapping.w);
    vec2 final_uv = vec2(u, v);
    final_uv = vec2(final_uv.x, final_uv.y);
    vec4 texture_color = texture2D(albedo, final_uv);
    if (texture_color.w == 0) discard;
    color = color_mult * texture_color;
}

)"""";