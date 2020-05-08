#version 150
in vec3 position;
in vec3 normal;
out vec3 norm;
out vec3 fragPos;

uniform mat4 matrix;
uniform mat4 model;
uniform mat3 rotation;
void main()
{
    gl_Position = matrix * vec4(position.x, position.y, position.z, 1.0);
    norm = rotation * normal;
    fragPos = vec3(model * vec4(position, 1.0));
}