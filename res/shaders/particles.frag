#version 450 core

in vec2 textureCoordinates;
in vec4 particleColor;
out vec4 color;

uniform sampler2D textureIn;

void main()
{
    color = vec4(1,0,0,1);
}