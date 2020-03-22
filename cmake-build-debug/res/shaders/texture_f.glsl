#version 150 core
in vec2 texCoord;
out vec4 outColor;

uniform vec4 color;
uniform sampler2D sampler;

void main() {
    vec4 v = texture2D(sampler, texCoord);
    outColor = vec4(v.rgb * color.rgb, 1) * v.a * color.a;
}