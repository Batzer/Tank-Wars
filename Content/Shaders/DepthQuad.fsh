#version 330 core

out vec4 colorF;
in vec2 TexCoords;

uniform sampler2D DepthMap;

void main() {             
    float depthValue = texture(DepthMap, TexCoords).r;
    colorF = vec4(vec3(depthValue), 1.0);
} 