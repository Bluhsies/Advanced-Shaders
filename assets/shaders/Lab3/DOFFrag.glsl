#version 460 core

layout(location = 0) out vec4 depthColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform sampler2D u_depthTexture;
uniform sampler2D u_blurTexture;
uniform float u_near;
uniform float u_far;
uniform float u_focus;
uniform float u_depthFallOff;

float LineariseDepth(float depth);

//This is the Depth of Field fragment shader.
//It makes use of the depth texture and a focus value, which is used to determine the distance
//the viewer is able to see a clearer image.

void main()
{

	vec3 colourValue = texture(u_originalTexture, texCoord).rgb;
	vec3 blurValue = texture(u_blurTexture, texCoord).rgb;

	float depthValue = texture(u_depthTexture, texCoord).r;
	float linearDepth = LineariseDepth(depthValue);

	float focusDistance = abs(linearDepth - u_focus);

	float depthFocus = smoothstep(0.0, u_depthFallOff, focusDistance);

	depthColour = vec4(mix(colourValue, blurValue, depthFocus), 1);

}

//This function will linearise the depth float which is called from the depth texture, using the
//uniform values set for u_near and u_far.
float LineariseDepth(float depth)
{

	float z = depth * 2.0 - 1.0;
	return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));

}