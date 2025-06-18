#version 460 core

layout(location = 0) out vec4 depthColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform sampler2D u_depthTexture;
uniform float u_near;
uniform float u_far;
uniform vec3 u_fogColour;

float LineariseDepth(float depth);

//This is the fog fragment shader, which uses samples from the original texture and depth texture, as well
// as uniform values for the near, far and colour of the fog.
//There are two different variations of calculated fog: Linear and Exponential that can be tested.
void main()
{

	//Linear
	//float depthValue = texture(u_depthTexture, texCoord).r;
	//vec3 colourValue = texture(u_originalTexture, texCoord).rgb;

	//float linearDepth = LineariseDepth(depthValue);
	//float depthNormalised = (linearDepth - u_near) / (u_far - u_near);
		
	//depthColour = vec4(mix(colourValue, u_fogColour, depthNormalised), 1);

	//Exponential
	float depthValue = texture(u_depthTexture, texCoord).r;
	vec3 colourValue = texture(u_originalTexture, texCoord).rgb;

	float linearDepth = LineariseDepth(depthValue);
	float fogDensity = 0.05;
	float fogFactor = 1.0 - exp(-fogDensity * linearDepth);

	depthColour = vec4(mix(colourValue, u_fogColour, fogFactor), 1);

}

float LineariseDepth(float depth)
{

	float z = depth * 2.0 - 1.0;
	return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));

}