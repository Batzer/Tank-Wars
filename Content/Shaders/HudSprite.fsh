#version 330 core

in vec2 outTexCoord;

out vec4 colorF;

uniform float Transparency;
uniform sampler2D TextureSampler;

void main() {
    colorF = vec4(texture(TextureSampler, outTexCoord).rgb, Transparency);
}
