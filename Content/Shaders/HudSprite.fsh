#version 330 core

in vec2 outTexCoord;

out vec4 colorF;

uniform float Transparency;
uniform sampler2D TextureSampler;

void main() {
	vec4 texColor = texture(TextureSampler, outTexCoord);
    colorF = vec4(texColor.rgb, texColor.a * Transparency);
}
