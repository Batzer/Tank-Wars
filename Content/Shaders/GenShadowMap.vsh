#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewProjMatrix;

void main() {
    gl_Position = ViewProjMatrix * ModelMatrix * vec4(inPosition, 1.0);
}