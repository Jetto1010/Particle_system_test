#version 450 core

in vec2 textureCoordinates;

uniform layout(binding = 0) sampler2D textureIn;

out vec4 color;

void main()
{
    color = texture(textureIn, textureCoordinates) * vec4(1, 0, 0, 1);
}