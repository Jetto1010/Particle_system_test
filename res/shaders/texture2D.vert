#version 430 core

in layout (location = 0) vec3 position;
in layout (location = 2) vec2 inTextureCoordinates;
uniform layout (location = 3) mat4 ortho;

out vec2 textureCoordinates;

void main()
{
    textureCoordinates = inTextureCoordinates;
    gl_Position = ortho * vec4(position, 1.0);
}
