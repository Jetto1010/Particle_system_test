#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 textureCoordinates_in;
in layout(location = 3) vec3 tangent;
in layout(location = 4) vec3 bitangent;

uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 MVP;
uniform layout(location = 5) mat3 normalMat;

out layout(location = 0) vec3 normal_out;
out layout(location = 1) vec2 textureCoordinates_out;
out layout(location = 2) vec3 fragPos;
out layout(location = 3) mat3 TBN;

void main()
{
    vec3 T = normalize(normalMat * tangent);
    vec3 B = normalize(normalMat * bitangent);
    normal_out = normalize(normalMat * normal_in);
    TBN = mat3(T, B, normal_out);
    fragPos = vec3(model * vec4(position, 1));
    textureCoordinates_out = textureCoordinates_in;
    gl_Position = MVP * vec4(position, 1);
}
