#version 150 core
in vec2 texCoord;
out vec4 outColor;

uniform sampler2D sampler;
uniform vec4 color;

void main()
{
    float a = texture(sampler, texCoord).r;
    outColor = color * vec4(1, 1, 1, a);
}