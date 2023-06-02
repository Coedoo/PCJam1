#version 330 core

out vec4 color;

in vec2 uv;

void main() {
    color = vec4(uv.x, uv.y, 0, 1);
}