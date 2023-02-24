#version 430 core

in vec2 textureCoordinates;

uniform layout(binding = 0) sampler2D textureIn;

out vec4 outColour;

void main()
{
    outColour = texture(textureIn, textureCoordinates);
}