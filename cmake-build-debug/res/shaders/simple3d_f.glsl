#version 150
#define LIGHTS 8
in vec3 norm;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 ambiance;

uniform Light {
    vec3 position;
    vec3 color;
    float strength;
} lights[LIGHTS];

uniform Material {
    vec3 diffuse;
} material;

void main()
{
    vec3 positions[LIGHTS];
    vec3 colors[LIGHTS];
    float strengths[LIGHTS];
    positions[0] = lights[0].position;
    positions[1] = lights[1].position;
    positions[2] = lights[2].position;
    positions[3] = lights[3].position;
    positions[4] = lights[4].position;
    positions[5] = lights[5].position;
    positions[6] = lights[6].position;
    positions[7] = lights[7].position;
    colors[0] = lights[0].color;
    colors[1] = lights[1].color;
    colors[2] = lights[2].color;
    colors[3] = lights[3].color;
    colors[4] = lights[4].color;
    colors[5] = lights[5].color;
    colors[6] = lights[6].color;
    colors[7] = lights[7].color;
    strengths[0] = lights[0].strength;
    strengths[1] = lights[1].strength;
    strengths[2] = lights[2].strength;
    strengths[3] = lights[3].strength;
    strengths[4] = lights[4].strength;
    strengths[5] = lights[5].strength;
    strengths[6] = lights[6].strength;
    strengths[7] = lights[7].strength;

    vec4 v = vec4(material.diffuse, 1);

    vec3 ambient = ambiance * v.rgb;
    vec3 open = min(vec3(1), v.rgb) - ambient;

    vec3 diffuseLight = vec3(0);

    for(int i = 0; i < LIGHTS; ++i) {
        if(strengths[i] <= 0) {
            continue;
        }
        vec3 lightDist = positions[i] - fragPos;
        float diffuseStrength;
        if(strengths[i] >= 100000) {
            diffuseStrength = 1;
        } else {
            diffuseStrength = strengths[i] / (length(lightDist) * length(lightDist));
        }

        vec3 normal = normalize(norm);
        vec3 lightDir = normalize(lightDist);
        float diff = max(dot(normal, lightDir), 0);
        diffuseLight += diffuseStrength * diff * colors[i];
    }
    diffuseLight = min(vec3(1), diffuseLight);

    vec3 result = min(ambient + diffuseLight * open, vec3(1));

    fragColor = vec4(result, v.a);
}