#version 330 core

in vec4 outParticleColor;
in vec2 outTexCoord;

out vec4 colorF;

uniform sampler2D TextureSampler;

void main() {
    colorF = texture(TextureSampler, outTexCoord) * outParticleColor;
}
