#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec3 outNormal;
out vec2 outTexCoord;
out vec3 outWorldPos;
out vec4 outLightPos;

uniform mat4 ModelMatrix;
uniform mat4 InvTrModelMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 LightMatrix;

void main() {
    vec4 worldPos = ModelMatrix * vec4(inPosition, 1.0);
    outNormal = (InvTrModelMatrix * vec4(inNormal, 0.0)).xyz;
    outTexCoord = inNormal.xy * 0.5 + 0.5;
    outWorldPos = worldPos.xyz;
    outLightPos = LightMatrix * worldPos;
    gl_Position = ViewProjMatrix * worldPos;
}