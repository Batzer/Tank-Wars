#version 330 core

in vec3 outNormal;

out vec4 colorF;

uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 DirToLight;
uniform vec3 MaterialDiffuse;

void main()
{
    vec3 normal = normalize(outNormal);
    float NdotL = dot(normal, DirToLight);
    float diffuseFactor = max(NdotL, 0.0);
    colorF = vec4(AmbientColor * MaterialDiffuse + diffuseFactor * LightColor * MaterialDiffuse, 1.0);
}