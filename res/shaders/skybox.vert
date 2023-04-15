#version 450 core

in layout (location = 0) vec3 position;

uniform layout (location = 0) mat4 VP;
uniform layout (location = 1) vec3 offset;

out vec3 textureCoordinates;

void main()
{
    textureCoordinates = position;
    gl_Position = VP * vec4(position + offset, 1.0);
}