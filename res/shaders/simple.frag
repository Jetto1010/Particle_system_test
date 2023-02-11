#version 430 core

const int numLights = 3;
const float strengthAmbient = 0.1;
const float strengthDiffuse = 0.3;
const float strengthSpecular = 0.3;
const vec3 colourAmbient = vec3(1.0, 1.0, 1.0);
const vec3 colourDiffuse = vec3(1.0, 1.0, 1.0);
const vec3 colourSpecular = vec3(1.0, 1.0, 1.0);

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec4 fragPos;

uniform layout(location = 10) vec3 lightPositions;
uniform layout(location = 11) vec3 camera;

out vec4 color;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    vec3 normalizedNormal = normalize(normal);
    float diff = 0;
    float spec = 0;
    for (int i = 0; i < numLights; i++) {
        vec3 lightDirection = normalize(lightPositions[i] - vec3(fragPos));
        vec3 viewDirection = normalize(camera - vec3(fragPos));
        vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);

        diff += max(dot(normalizedNormal, lightDirection), 0);
        spec = pow(max(dot(viewDirection, reflectDirection), 0), 100);
    }

    vec3 ambient = strengthAmbient * colourAmbient;
    vec3 diffuse = diff * strengthDiffuse * colourDiffuse;
    vec3 specular = spec * strengthSpecular * colourSpecular;
    color = vec4((ambient + diffuse + specular) * 1.0, 1.0);
}