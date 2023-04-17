#version 450 core
in layout (location = 0) vec3 position;
in layout (location = 2) vec2 inTextureCoordinates;

uniform layout(location = 0) mat4 MVP;
uniform layout(location = 1) vec3 offset;
uniform layout(location = 2) vec4 color;
uniform layout(location = 3) float scale;

out vec2 textureCoordinates;
out vec4 particleColor;

void main()
{
    textureCoordinates = inTextureCoordinates;
    particleColor = color;
    gl_Position = MVP * vec4(position * scale + offset, 1.0);
}
