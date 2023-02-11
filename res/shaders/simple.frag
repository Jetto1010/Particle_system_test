#version 430 core

const int numLights = 3;
const float la = 0.001;
const float lb = 0.001;
const float lc = 0.001;
const float radius = 3;
const float strengthAmbient = 0.1;
const float strengthDiffuse = 0.3;
const float strengthSpecular = 0.6;
const vec3 colourAmbient = vec3(1, 1, 1);
const vec3 colourDiffuse = vec3(1, 1, 1);
const vec3 colourSpecular = vec3(1, 1, 1);

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 fragPos;

uniform layout(location = 6) vec3 lightPositions1;
uniform layout(location = 7) vec3 lightPositions2;
uniform layout(location = 8) vec3 lightPositions3;
uniform layout(location = 9) vec3 camera;
uniform layout(location = 10) vec3 ball;

out vec4 color;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }
vec3 reject(vec3 from, vec3 onto) { return from - onto*dot(from, onto)/dot(onto, onto); }

void main()
{
    float diff = 0;
    float spec = 0;
    float shadow = 1;
    vec3 lightPositions[numLights];
    vec3 normalizedNormal = normalize(normal);
    lightPositions[0] = lightPositions1;
    lightPositions[1] = lightPositions2;
    lightPositions[2] = lightPositions3;

    for (int i = 0; i < numLights; i++) {
        vec3 fragmentBallVector = ball - fragPos;
        vec3 fragmentLightVector = lightPositions[i] - fragPos;
        vec3 rejection = reject(fragmentBallVector, fragmentLightVector);

        if (!(length(fragmentLightVector) < length(fragmentBallVector)|| dot(fragmentBallVector, fragmentLightVector) < 0 || length(rejection) > radius)) {
            continue;
        }

        vec3 lightDirection = normalize(lightPositions[i] - fragPos);
        vec3 viewDirection = normalize(camera - fragPos);
        vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);

        float d = length(lightPositions[i] - fragPos);
        float attenuation = 1 / (la + d * lb + d * d * lc);
        diff += max(dot(normalizedNormal, lightDirection), 0);
        spec += pow(max(dot(viewDirection, reflectDirection), 0), 100) * attenuation;
    }

    vec3 ambient = strengthAmbient * colourAmbient;
    vec3 diffuse = diff * strengthDiffuse * colourDiffuse;
    vec3 specular = spec * strengthSpecular * colourSpecular;
    color = vec4((ambient + diffuse + specular + dither(textureCoordinates)), 1.0);
}