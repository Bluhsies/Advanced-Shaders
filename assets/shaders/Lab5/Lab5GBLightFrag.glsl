#version 460 core

layout(location = 0) out vec4 colour;

in vec2 texCoord;

uniform sampler2D gBufferPosWS;
uniform sampler2D gBufferNormals;
uniform sampler2D gBufferDiffSpec;
uniform sampler2D gBufferDepth;
uniform sampler2D skyBoxCol;
uniform sampler2D u_shadowMap;
uniform mat4 u_lightSpaceTransform;
uniform vec2 gMapSize;


struct directionalLight
{
	vec3 colour;
	vec3 direction;
};

struct pointLight
{
	vec3 colour;
	vec3 position;
	vec3 constants;
};

struct spotLight
{
	vec3 colour;
	vec3 position;
	vec3 direction;
	vec3 constants;
	float cutOff;
	float outerCutOff;
};

const int numPointLights = 10;
const int numSpotLights = 1;

layout(std140, binding = 1) uniform b_lights
{
	uniform directionalLight dLight;
	uniform pointLight pLights[numPointLights];
	uniform spotLight sLights[numSpotLights];
};

layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


vec3 getDirectionalLight();
vec3 getPointLight(int idx);
vec3 getSpotLight(int idx);
float ShadowCalculation(vec3 worldPosition);

//global vars
vec3 normal;
float specularStrength;
vec3 viewDir;
vec3 fragmentPos;
vec3 diffColour;
float depth;
vec3 SBColour;


void main()
{

	fragmentPos = texture(gBufferPosWS, texCoord).rgb;
	normal = texture(gBufferNormals, texCoord).rgb;
	diffColour = texture(gBufferDiffSpec, texCoord).rgb;
	specularStrength = texture(gBufferDiffSpec, texCoord).a;
	depth = texture(gBufferDepth, texCoord).r;
	SBColour = texture(skyBoxCol, texCoord).rgb;
	//The major difference is that we sample the skybox texture in the lightpass

	vec3 result = vec3(0.0, 0.0, 0.0);
	viewDir = normalize(u_viewPos - fragmentPos);

	// light casters

	result += getDirectionalLight();

	for (int i = 0; i < numPointLights; i++)
	{
		result += getPointLight(i);
	}

	for (int i = 0; i < numSpotLights; i++)
	{
		result += getSpotLight(i);
	}

	colour = vec4(result, 1.0) * vec4(diffColour,1);

	if (depth >= 0.9999f)
	{
		colour = vec4(SBColour, 1.0);
	}
}


vec3 getDirectionalLight()
{
	float shadowAmount = ShadowCalculation(fragmentPos);

	float ambientStrength = 0.09;
	vec3 ambient = ambientStrength * dLight.colour;

	float diffuseFactor = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diffuseFactor * dLight.colour;

	vec3 H = normalize(-dLight.direction + viewDir);
	float specularFactor = pow(max(dot(normal, H), 0.0), 64);
	vec3 specular = dLight.colour * specularFactor * specularStrength;

	return ambient + (1.0 - shadowAmount) * (diffuse + specular);

	
}

vec3 getPointLight(int idx)
{
	float ambientStrength = 0.09;
	vec3 ambient = ambientStrength * pLights[idx].colour;

	float distance = length(pLights[idx].position - fragmentPos);
	float attn = 1.0 / (pLights[idx].constants.x + (pLights[idx].constants.y * distance) + (pLights[idx].constants.z * (distance * distance)));
	vec3 lightDir = normalize(pLights[idx].position - fragmentPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * attn * pLights[idx].colour;

	vec3 H = normalize(lightDir + viewDir);
	float specularFactor = pow(max(dot(normal, H), 0.0), 64);
	vec3 specular = pLights[idx].colour * specularFactor * specularStrength;

	return ambient + diffuse + specular;
}

vec3 getSpotLight(int idx)
{
	vec3 lightDir = normalize(sLights[idx].position - fragmentPos);
	float theta = dot(lightDir, normalize(-sLights[idx].direction));
	float ambientStrength = 0.09;
	vec3 ambient = ambientStrength * sLights[idx].colour;

	if (theta > sLights[idx].outerCutOff)
	{

		float distance = length(sLights[idx].position - fragmentPos);
		float attn = 1.0 / (sLights[idx].constants.x + (sLights[idx].constants.y * distance) + (sLights[idx].constants.z * (distance * distance)));
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * attn * sLights[idx].colour;

		float specularStrength = 0.08;

		vec3 H = normalize(lightDir + viewDir);
		float specularFactor = pow(max(dot(normal, H), 0.0), 64);
		vec3 specular = pLights[idx].colour * specularFactor * specularStrength;

		float epsilon = sLights[idx].cutOff - sLights[idx].outerCutOff;
		float intensity = clamp((theta - sLights[idx].outerCutOff) / epsilon, 0.0, 1.0);

		diffuse *= intensity;
		specular *= intensity;

		return (ambient + diffuse + specular);
	}
	else
	{
		return ambient;
	}
}


float ShadowCalculation(vec3 worldPosition)
{

	vec4 fragPosLightSpace = u_lightSpaceTransform * vec4(worldPosition, 1.0);

	//vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//projCoords = projCoords * 0.5 + 0.5;

	//float closestDepth = texture(u_shadowMap, projCoords.xy).r;

	//float currentDepth = projCoords.z;

	//float bias = 0.0015;

	//float shadow = 0.0;

	//if(currentDepth-bias > closestDepth)
	//{
		//shadow = 1.0;
	//}

	//return shadow;


	//PCF one
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	//Early return if outside shadow map bounds
	if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
	    return 0.0;
	}

	float currentDepth = projCoords.z;
	//float bias = max(0.05 * (1.0 - dot(normal, -dLight.direction)), 0.005); // Depth bias

	float bias = 0.0000010;

	// PCF kernel
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0); // Shadow map resolution
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float closestDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			if (currentDepth - bias > closestDepth) {
			    shadow += 1.0;
			}
		}
	}
	shadow /= 9.0; // Average over the 3x3 kernel

	if(projCoords.z > 1.0) shadow = 0.0;

	return shadow;

	//PCF two

	

	//vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//projCoords = projCoords * 0.5 + 0.5;


	//vec2 UVCoords;
	//UVCoords.x = 0.5 * projCoords.x + 0.5;
	//UVCoords.y = 0.5 * projCoords.y + 0.5;
	//float currentDepth = 0.5 * projCoords.z + 0.5;

	//float xOffset = 1.0 / gMapSize.x;
	//float yOffset = 1.0 / gMapSize.y;

	//float Factor = 0.0;
	//for (int y = -1; y <= 1; y++) {

		//for (int x = -1; x <= 1; x++) {

			//vec2 Offsets = vec2(x * xOffset, y * yOffset);
			//vec2 sampleUV = UVCoords + Offsets;

			//float closestDepth = texture(u_shadowMap, sampleUV).r;

			//if (currentDepth - 0.0015 > closestDepth) {

				//Factor += 1.0;

			//}

		//}

	//}

	//float shadow = Factor / 9.0;

	//return shadow;

}