#version 450 core
in layout (location = 0) vec3 position;
in layout (location = 2) vec2 inTextureCoordinates;

uniform layout (location = 0) mat4 MVP;

out vec2 textureCoordinates;

void main()
{
    textureCoordinates = inTextureCoordinates;
    gl_Position = MVP * vec4(position, 1.0);
}