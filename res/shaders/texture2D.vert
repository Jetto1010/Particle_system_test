#version 430 core

in layout (location = 0) vec3 posistion;
in layout (location = 1) vec2 textureCoordinates;
uniform layout (location = 2) mat4 ortho;

out vec2 outTextureCoordinates;

void main()
{
    outTextureCoordinates = textureCoordinates;
    gl_Position = ortho * vec4(posistion, 1.0);
}
