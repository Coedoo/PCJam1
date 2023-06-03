#version 330 core

out vec4 color;

in vec2 uv;
in vec2 uv2;

void main() {
    float mX = abs(uv2.x * 2 - 1);
    float mY = abs(uv2.y * 2 - 1);

    vec3 cX = vec3(1, 0, 1) * mX * mX * mX;
    vec3 cY = vec3(1, 0, 1) * mY * mY * mY;

    color = vec4(max(cX, cY), 1);
}