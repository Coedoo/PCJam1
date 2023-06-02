#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 color;

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

    float sdf = sdCircle(uv - sunPos, sunSize);
    
    float s = step(sin(uv.y * 40.0), 0.0);
    
    
    vec3 c = mix(bg, vec3(1, 1, 1)*s, step(sdf, 0.0));
    
    color = vec4(c, 1);
}