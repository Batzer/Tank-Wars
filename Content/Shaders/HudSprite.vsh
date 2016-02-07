#version 330 core

layout(location = 0) in vec3 inPosition;

out vec2 outTexCoord;

uniform vec4 Dimensions; // xy = pos, zw = width/height
uniform vec4 TexDimensions; // xy = pos, zw = width/height

void main() {
    vec2 pos = inPosition.xy * Dimensions.zw + Dimensions.xy;
	outTexCoord = (inPosition.xy + vec2(0.5, 0.5)) * TexDimensions.zw + TexDimensions.xy;
    gl_Position = vec4(pos.x, pos.y, inPosition.z, 1.0);
}
