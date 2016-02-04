#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewProjMatrix;

void main() {
    vec4 pos = ViewProjMatrix * ModelMatrix * vec4(inPosition, 1.0);
    vec4 normal = ViewProjMatrix * ModelMatrix * vec4(inNormal, 0.0);
    gl_Position = pos + normal * 0.035;
}