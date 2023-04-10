#version 450 core

in vec3 textureCoordinates;

uniform samplerCube box;

out vec4 color;

void main()
{
    color = texture(box, textureCoordinates);
}