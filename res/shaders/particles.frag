#version 430 core

in vec2 textureCoordinates;
in vec4 particleColor;
out vec4 color;

uniform layout(binding = 0) sampler2D textureIn;

void main()
{
    color = texture(textureIn, textureCoordinates);
}