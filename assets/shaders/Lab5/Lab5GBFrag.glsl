#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;

in vec3 vertexNormal;
in vec3 fragmentPos;
in vec4 fragPosClipSpace;
in vec2 texCoord;
in mat3 TBN;

uniform sampler2D u_albedoMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;


vec3 normal;
vec3 albedoColour;
float specularStrength = 0.8;


void main()
{


	gPosition = vec4(fragmentPos, 1.0);

	vec3 N = texture(u_normalMap, texCoord).rgb;
	normal = normalize(TBN * (N * 2.0 - 1.0));
	gNormal = vec4(normal, 0.0);

	float specularStrength = texture(u_specularMap, texCoord).r;
	albedoColour = texture(u_albedoMap, texCoord).rgb;
	gDiffSpec = vec4(albedoColour, specularStrength);




}