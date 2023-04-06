#version 430 core
in layout (location = 0) vec3 position;
in layout (location = 2) vec2 inTextureCoordinates;

uniform layout(location = 2) mat4 MVP;
uniform layout(location = 3) vec3 offset;
uniform layout(location = 4) vec4 color;

out vec2 textureCoordinates;
out vec4 particleColor;

void main()
{
    float scale = 10.0f;
    textureCoordinates = inTextureCoordinates;
    particleColor = color;
    gl_Position = MVP * vec4((position * scale) + offset, 1.0);
}
