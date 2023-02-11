#version 430 core

const int numLights = 3;
const float strengthAmbient = 0.1;
const float strengthDiffuse = 0.5;
const vec3 colourAmbient = vec3(0.1, 0.1, 0.1);
const vec3 colourDiffuse = vec3(1.0, 1.0, 1.0);

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec4 fragPos;

uniform layout(location = 10) vec3 lightPosition[numLights];

out vec4 color;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    vec3 normalizedNormal = normalize(normal);
    float diff = 0;
    for (int i = 0; i < numLights; i++) {
        vec3 lightDirection = normalize(lightPosition[i] - vec3(fragPos));
        diff = max(dot(normalizedNormal, lightDirection), 0);
    }

    vec3 ambient = strengthAmbient * colourAmbient;
    vec3 diffuse = diff * colourDiffuse;
    color = vec4((ambient + diffuse) * 1.0, 1.0);
}