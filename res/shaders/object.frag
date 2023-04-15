#version 450 core

in vec2 textureCoordinates;

uniform sampler2D textureIn;

out vec4 color;

void main()
{
    color = texture(textureIn, textureCoordinates);
}