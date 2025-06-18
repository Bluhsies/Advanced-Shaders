#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;

// This is my fragment shader to calculate relative luminance.
// There are two version, regular and the adjusted RGB version.

void main()
{

	//vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	//float luminance = 0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b;
	//ppColour = vec4(vec3(luminance), 1);

	vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	float luminance = 0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b;
	vec3 adjustedRGB = pow(vec3(luminance), vec3(1.5,0.8,1.5));
	ppColour = vec4(adjustedRGB, 1);

}