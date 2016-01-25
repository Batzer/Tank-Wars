#version 330 core

in vec3 outNormal;
in vec3 outWorldPos;

out vec4 colorF;

uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 DirToLight;
uniform vec3 EyePos;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float SpecularExponent;

void main() {
	vec3 color = AmbientColor * MaterialDiffuse;
    vec3 N = normalize(outNormal);
    float NdotL = dot(N, DirToLight);

	if (NdotL > 0.0) {
		vec3 V = normalize(EyePos - outWorldPos);
		vec3 H = normalize(V + DirToLight);
		float diffuseFactor = max(NdotL, 0.0);
		if (diffuseFactor < 0.1) diffuseFactor = 0.0;
		else if (diffuseFactor < 0.3) diffuseFactor = 0.3;
		else if (diffuseFactor < 0.6) diffuseFactor = 0.6;
		else diffuseFactor = 1.0;
		float specularFactor = step(0.5, pow(max(dot(N, H), 0.0), SpecularExponent));
		color += diffuseFactor * MaterialDiffuse + specularFactor * MaterialSpecular;
	}

    colorF = vec4(color * LightColor, 1.0);
}