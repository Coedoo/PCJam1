#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 color;

uniform float time;
uniform vec2 resolution;

float sdCircle( vec2 p, float r ) {
    return length(p) - r;
}

void main() {
    vec2 fragCoord = vec2(resolution.x * fragTexCoord.x, resolution.y * fragTexCoord.y);
    vec2 uv = (2.0f * fragCoord - resolution.xy) / resolution.y;
    
    vec2 sunPos = vec2(0.8, 0.3);
    float sunSize = 0.5;


    vec3 bg = mix(vec3(0.8,0,0.5), vec3(0.8,0.3, 0.8), uv.y);
    
    vec2 sunUV = uv - sunPos;
    float sdf = sdCircle(sunUV, sunSize);
  
    float s = step(cos(uv.y * 40.0 + time), 0.0) + 0.7f;
    vec3 sunColor = mix(vec3(0.93, 0.418, 0.013), vec3(0.995, 0.812, 0.174), (sunUV.y + 0.5)) * s;
  
    vec3 c = mix(bg, sunColor, smoothstep(0.03, 0.0, sdf));
    
    color = vec4(c, 1);
}