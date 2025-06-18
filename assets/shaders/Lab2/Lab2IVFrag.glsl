#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;

//This shader inverts the colour of the screen, but taking away the value of rgb from 1.0,
//producing a negative appearance.

void main()
{

	vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	rgb = 1.0 - rgb;
	ppColour = vec4(rgb, 1);

}