#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;

in vec3 fragmentPos;
in vec3 vertexNormal;
in vec2 texCoord;
in vec4 fragPosClipSpace;
in vec3 CDMnormal;
in float scale;

uniform sampler2D u_albedoMap;
//uniform vec3 u_albedo;
uniform bool u_normalSelect;

vec3 normal;
vec3 albedoColour;

//Contains outputs for per face normal and the per vertex normal. Per vertex provides a smoother looking
//output which is nicer, whilst the per face normal provides a low poly output.
void main()
{

	//Provides a max scale factor for the height of the floor.
	float mid = scale * 0.5;

	//Used to two-tone the floor colouring based on heightmap.
	vec3 col1 = vec3(0.2,0.6,0.1);
	vec3 col2 = vec3(0.6,0.2,0.1);


	
	//Per Face normal
	//normal = normalize(vertexNormal);
	//gNormal = vec4(normal, 1.0);

	gNormal = vec4(CDMnormal, 1.0); // Per Vertex normal

	//albedoColour = texture(u_albedoMap, texCoord).rgb;
	albedoColour = mix(col1,col2, smoothstep(0.0, mid + mid*0.5, fragmentPos.y));
	gPosition = vec4(fragmentPos, 1.0);	
	gDiffSpec = vec4(albedoColour, 1.0) * texture(u_albedoMap, texCoord);


}