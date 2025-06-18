#version 460 core

layout(location = 0) out vec4 colour;

//This fragment shader is for the model and a separate shader has been made for the floor.

in vec3 vertexNormal;
in vec3 fragmentPos;
in vec4 fragPosClipSpace;
in vec4 fragPosLightSpace;
in vec2 texCoord;
in mat3 TBN;

uniform sampler2D u_albedoMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthMap;
uniform sampler2D u_shadowMap;
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


//global vars
vec3 normal;
vec3 albedoColour;
float specularStrength = 0.8;
vec3 viewDir;

bool doesPassDepthTest();
float ShadowCalculation(vec4 fragPosLightSpace);


void main()
{

	if(!doesPassDepthTest()) return;

	vec3 result = vec3(0.0, 0.0, 0.0);
	viewDir = normalize(u_viewPos - fragmentPos);
	normal = normalize(texture(u_normalMap, texCoord).rgb);

	specularStrength = texture(u_specularMap, texCoord).r;
	albedoColour = texture(u_albedoMap, texCoord).rgb;

	vec3 N = texture(u_normalMap, texCoord).rgb;
	normal = normalize(TBN * (N * 2.0 - 1.0));

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

	//Removed the * u_albedo as I am not using this parameter for the model.
	colour = vec4(result, 1.0)* texture(u_albedoMap, texCoord);

	
	

}


vec3 getDirectionalLight()
{
	//Add a shadow amount to the ambient light, minused from 1.0 times the diffuse + specular. (as shown in the return)
	float shadowAmount = ShadowCalculation(fragPosLightSpace);

	float ambientStrength = 0.09;
	vec3 ambient = ambientStrength * dLight.colour;

	float diffuseFactor = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diffuseFactor * dLight.colour;

	vec3 H = normalize(-dLight.direction + viewDir);
	float specularFactor = pow(max(dot(normal, H), 0.0), 64);
	vec3 specular = dLight.colour * specularFactor * specularStrength;

	return ambient + (1.0 - shadowAmount)*(diffuse + specular);
}

vec3 getPointLight(int idx)
{
	float ambientStrength = 0.01;
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
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * sLights[idx].colour;

	if (theta > sLights[idx].outerCutOff)
	{

		float distance = length(sLights[idx].position - fragmentPos);
		float attn = 1.0 / (sLights[idx].constants.x + (sLights[idx].constants.y * distance) + (sLights[idx].constants.z * (distance * distance)));
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * attn * sLights[idx].colour;

		float specularStrength = 0.01;

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

bool doesPassDepthTest()
{

	float fragmentDepth = fragPosClipSpace.z / fragPosClipSpace.w;

	fragmentDepth = fragmentDepth * 0.5 + 0.5;

	vec2 fragCoord = (fragPosClipSpace.xy / fragPosClipSpace.w) * 0.5 + 0.5;

	float depthPrepass = texture(u_depthMap, fragCoord).r;

	float bias = 0.001;

	if(fragmentDepth >= depthPrepass + bias) return false;

	return true;

}

//This function is used to calculate the shadows in the scene for the floor.
//There are three iterations: original, PCF1 and PCF2.
//PCF provides shadows with the smoothest edges, whilst also removing shadow acne.
float ShadowCalculation(vec4 fragPosLightSpace)
{

	//vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//projCoords = projCoords * 0.5 + 0.5;

	//float closestDepth = texture(u_shadowMap, projCoords.xy).r;

	//float currentDepth = projCoords.z;

	//float bias = 0.015;

	//float shadow = 0.0;

	//if(currentDepth-bias > closestDepth)
	//{
		//shadow = 1.0;
	//}

	//return shadow;

	
	//PCF one
	//vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //projCoords = projCoords * 0.5 + 0.5;

    // Early return if outside shadow map bounds
    //if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) {
       // return 0.0;
    //}

    //float currentDepth = projCoords.z;
    //float bias = max(0.05 * (1.0 - dot(normal, -dLight.direction)), 0.005); // Depth bias

    // PCF kernel
    //float shadow = 0.0;
    //vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0); // Shadow map resolution
    //for (int x = -1; x <= 1; ++x) {
        //for (int y = -1; y <= 1; ++y) {
            //float closestDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            //if (currentDepth - bias > closestDepth) {
               // shadow += 1.0;
            //}
        //}
    //}
    //shadow /= 9.0; // Average over the 3x3 kernel

	//if(projCoords.z > 1.0) shadow = 0.0;

    //return shadow;

	//PCF two
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	vec2 UVCoords;
	UVCoords.x = 0.5 * projCoords.x + 0.5;
	UVCoords.y = 0.5 * projCoords.y + 0.5;
	float currentDepth = 0.5 * projCoords.z + 0.5;

	float xOffset = 1.0 / gMapSize.x;
	float yOffset = 1.0 / gMapSize.y;

	float Factor = 0.0;
	for(int y = -1; y<= 1; y++) {
	
		for(int x = -1; x <= 1; x++) {
		
			vec2 Offsets = vec2(x * xOffset, y * yOffset);
			vec2 sampleUV = UVCoords + Offsets;

			float closestDepth = texture(u_shadowMap, sampleUV).r;

			if(currentDepth - 0.015 > closestDepth) {
			
				Factor += 1.0;
			
			}

		}

	}

	float shadow = Factor / 9.0;

	return shadow;

}