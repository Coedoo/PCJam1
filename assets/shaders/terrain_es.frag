#version 100
precision mediump float;

// out vec4 color;

varying vec2 uv;
varying vec2 uv2;

float smootherstep(float x, float edge0, float edge1) {
  // Scale, and clamp x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);

  return x * x * x * (3.0 * x * (2.0 * x - 5.0) + 10.0);
}


float edgeFun(float m) {
    return smootherstep(m, 0.9, 1.0);
}

void main() {
    float mX = abs(uv2.x * 2.0 - 1.0);
    float mY = abs(uv2.y * 2.0 - 1.0);

    vec3 edgeCol = vec3(1.0, 0.0, 1.0);
    vec3 surfaceColor = vec3(0.1, 0.0, 0.1);

    vec3 edge = (edgeFun(mX) + edgeFun(mY)) * edgeCol;
    vec3 c = surfaceColor + edge;

    gl_FragColor = vec4(c, 1.0);
}