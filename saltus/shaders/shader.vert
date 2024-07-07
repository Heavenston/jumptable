#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform Ubo {
    mat4 mvp;
    float time;
} ubo;

void main() {
    gl_Position = ubo.mvp * vec4(pos, 0.0, 1.0);
    // gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = color;
}
