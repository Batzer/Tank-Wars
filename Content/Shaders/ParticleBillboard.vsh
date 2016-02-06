#version 330 core

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec4 inParticlePosition; // Instanced, w is particle size
layout(location = 2) in vec4 inParticleColor; // Instanced

out vec4 outParticleColor;
out vec2 outTexCoord;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 ViewProjMat;

void main() {
	vec3 vertexPos = inParticlePosition.xyz
		+ inVertexPosition.x * inParticlePosition.w * CameraRight
		+ inVertexPosition.y * inParticlePosition.w * CameraUp;

	outParticleColor = inParticleColor;
	outTexCoord = inVertexPosition.xy + vec2(0.5, 0.5);
    gl_Position = ViewProjMat * vec4(vertexPos, 1.0);
}
