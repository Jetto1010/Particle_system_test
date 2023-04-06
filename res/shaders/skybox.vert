#version 430 core

in layout (location = 0) vec3 position;

uniform layout (location = 0) mat4 VP;

out vec3 textureCoordinates;


void main()
{
    textureCoordinates = position;
    vec4 pos = VP * vec4(position, 1.0);
    gl_Position = pos.xyww;
}