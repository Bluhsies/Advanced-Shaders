#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;

in vec3 fragmentPos;
in vec3 vertexNormal;
in vec2 texCoord;
in vec4 fragPosClipSpace;

uniform sampler2D u_albedoMap;
uniform vec3 u_albedo;

vec3 normal;
vec3 albedoColour;

void main()
{


	normal = normalize(vertexNormal);
	albedoColour = texture(u_albedoMap, texCoord).rgb;

	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 1.0);
	gDiffSpec = vec4(u_albedo, 1.0) * texture(u_albedoMap, texCoord);


}