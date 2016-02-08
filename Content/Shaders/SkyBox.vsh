#version 330 core

layout(location = 0) in vec3 inPosition;
out vec3 outTexCoord;
uniform mat4 ViewProjMat;

void main() {
    gl_Position = (ViewProjMat * vec4(inPosition, 1.0)).xyww;
    outTexCoord = inPosition;
}
