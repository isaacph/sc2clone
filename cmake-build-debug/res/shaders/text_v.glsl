#version 150 core
in vec2 position;
in vec2 texture;
out vec2 texCoord;
uniform mat4 matrix;

void main() {
    gl_Position = matrix * vec4(position, 0.0, 1.0);
    texCoord = texture;
}