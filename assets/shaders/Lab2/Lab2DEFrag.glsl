#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform vec2 screenSize;
uniform float threshold;

//This is the edge detection shader.
//It takes in a uniform value for threshold, which can be amended in the GUI to view different output values.
//The output is a pink screen with the model having white lines to represent the edges.

void main()
{

	vec2 inverseScreen = 1.0 / screenSize;

	vec3 colourCentre = texture(u_originalTexture, texCoord).rgb;
	vec3 colourLeft = texture(u_originalTexture, texCoord + vec2(-1.0, 0.0) * inverseScreen).rgb;
	vec3 colourRight = texture(u_originalTexture, texCoord + vec2(1.0, 0.0) * inverseScreen).rgb;
	vec3 colourUp = texture(u_originalTexture, texCoord + vec2(0.0, 1.0) * inverseScreen).rgb;
	vec3 colourDown = texture(u_originalTexture, texCoord + vec2(0.0, -1.0) * inverseScreen).rgb;

	float lumCentre = dot(colourCentre, vec3(0.2126, 0.7152, 0.0722));
	float lumLeft = dot(colourLeft, vec3(0.2126, 0.7152, 0.0722));
	float lumRight = dot(colourRight, vec3(0.2126, 0.7152, 0.0722));
	float lumUp = dot(colourUp, vec3(0.2126, 0.7152, 0.0722));
	float lumDown = dot(colourDown, vec3(0.2126, 0.7152, 0.0722));

	float edgeHorizontal = abs(lumLeft - lumRight);
	float edgeVertical = abs(lumUp - lumDown);
	float edgeStrength = max(edgeHorizontal, edgeVertical);

	ppColour = mix(vec4(1, 0, 1, 1), vec4(1), step(threshold, edgeStrength));

}