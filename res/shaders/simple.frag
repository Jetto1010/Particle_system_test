#version 430 core

struct LightSource {
    vec3 lightPosition;
    vec3 colour;
};

const int numLights = 1;
const float la = 0.001;
const float lb = 0.001;
const float lc = 0.001;
const float radius = 3;
const float ambient = 0.1;
const float strengthDiffuse = 0.8;
const float strengthSpecular = 0.2;

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 fragPos;
in layout(location = 3) mat3 TBN;

uniform LightSource sources[numLights];
uniform layout(location = 9) vec3 camera;
uniform layout(location = 11) bool hasTexture;
uniform layout(location = 12) bool hasNormalMap;

uniform layout(binding = 0) sampler2D textureIn;
uniform layout(binding = 1) sampler2D normalMapIn;

out vec4 color;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }
vec3 reject(vec3 from, vec3 onto) { return from - onto*dot(from, onto)/dot(onto, onto); }

void main()
{
    vec3 diff = vec3(0);
    vec3 spec = vec3(0);
    vec3 normalizedNormal = normalize(normal);
    vec3 textureSample = vec3(1);

    if (hasTexture) {
         textureSample = vec3(texture(textureIn, textureCoordinates));
    }
    if (hasNormalMap) {
        normalizedNormal = TBN * (vec3(texture(normalMapIn, textureCoordinates)) * 2 - 1);
    }

    for (int i = 0; i < numLights; i++) {
        LightSource source = sources[i];

        /*vec3 fragmentBallVector = ball - fragPos;
        vec3 fragmentLightVector = source.lightPosition - fragPos;
        vec3 rejection = reject(fragmentBallVector, fragmentLightVector);
        if (!(length(fragmentLightVector) < length(fragmentBallVector)|| dot(fragmentBallVector, fragmentLightVector) < 0 || length(rejection) > radius)) {
            continue; // Skips rest of calcualtions
        }*/

        vec3 lightDirection = normalize(source.lightPosition- fragPos);
        vec3 viewDirection = normalize(camera - fragPos);
        vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);

        float d = length(source.lightPosition - fragPos);
        float attenuation = 1 / (la + d * lb + d * d * lc);
        diff += max(dot(normalizedNormal, lightDirection), 0) * attenuation * source.colour * textureSample;
        spec += pow(max(dot(viewDirection, reflectDirection), 0), 100) * attenuation * source.colour;
    }

    vec3 diffuse = diff * strengthDiffuse;
    vec3 specular = spec * strengthSpecular;
    color = vec4((ambient + diffuse + specular + dither(textureCoordinates)), 1.0);
}