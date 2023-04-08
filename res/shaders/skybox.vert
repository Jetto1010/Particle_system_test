#version 430 core

in layout (location = 0) vec3 position;

uniform layout (location = 0) mat4 VP;

out vec3 textureCoordinates;


void main()
{
    textureCoordinates = position;
    gl_Position = VP * vec4(position, 1.0);
}