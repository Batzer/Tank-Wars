#version 330 core

in vec3 outTexCoord;
out vec4 colorF;
uniform samplerCube SkyBox;

void main() {
    colorF = texture(SkyBox, outTexCoord);
}
