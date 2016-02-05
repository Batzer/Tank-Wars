#version 330 core

in vec3 outNormal;
in vec3 outWorldPos;
in vec4 outLightPos;

out vec4 colorF;

uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform vec3 DirToLight;
uniform vec3 EyePos;
uniform vec3 MaterialDiffuse;
uniform vec3 MaterialSpecular;
uniform float SpecularExponent;
uniform sampler2DShadow ShadowMap;

void main() {
	vec3 color = AmbientColor * MaterialDiffuse;
    vec3 N = normalize(outNormal);
    float NdotL = dot(N, DirToLight);

	if (NdotL > 0.0) {
		float diffuseFactor = max(NdotL, 0.0);
		if (diffuseFactor < 0.1) diffuseFactor = 0.0;
		else if (diffuseFactor < 0.3) diffuseFactor = 0.3;
		else if (diffuseFactor < 0.6) diffuseFactor = 0.6;
		else diffuseFactor = 1.0;

		vec3 V = normalize(EyePos - outWorldPos);
		vec3 H = normalize(V + DirToLight);
		float specularFactor = step(0.5, pow(max(dot(N, H), 0.0), SpecularExponent));

		vec3 projCoords = (outLightPos.xyz / outLightPos.w) * 0.5 + 0.5;
		float shadow = 0.0;

		if (projCoords.z > 1.0) {
			shadow = 1.0;
		}
		else {
			vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
			for (int x = -1; x <= 1; x++) {
				for (int y = -1; y <= 1; y++) {
					vec2 offset = vec2(x, y) * texelSize;
					shadow += texture(ShadowMap, vec3(projCoords.xy + offset, projCoords.z - 0.005));      
				}    
			}
			shadow /= 9.0;
		}

		color += (diffuseFactor * MaterialDiffuse + specularFactor * MaterialSpecular) * shadow;
	}

    colorF = vec4(color * LightColor, 1.0);
}