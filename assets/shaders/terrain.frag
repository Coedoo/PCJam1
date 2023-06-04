#version 330 core

out vec4 color;

in vec4 clipPos;
in vec2 uv;
in vec2 uv2;

float smootherstep(float x, float edge0 = 0.0f, float edge1 = 1.0f) {
  // Scale, and clamp x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0, 1);

  return x * x * x * (3.0f * x * (2.0f * x - 5.0f) + 10.0f);
}


float edgeFun(float m) {
    return smootherstep(m, 0.9, 1);
}

void main() {
    float mX = abs(uv2.x * 2 - 1) + (1 - clipPos.z) * 0.0007;
    float mY = abs(uv2.y * 2 - 1) + (1 - clipPos.z) * 0.0007;

    vec3 edgeCol = vec3(1, 0, 1);
    vec3 surfaceColor = vec3(0.1, 0, 0.1);

    vec3 edge = (edgeFun(mX) + edgeFun(mY)) * edgeCol;
    vec3 c = surfaceColor + edge;

    color = vec4(c, 1);
}