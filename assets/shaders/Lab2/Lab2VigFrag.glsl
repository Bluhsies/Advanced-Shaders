#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform vec2 screenCentre;

//This is the shader for my vignette post-processing.
//It takes the original texture and calculates how much of the screen
//Should produce a black ring around the centre. (the edges form into a circle which looks like a focussing lens)

void main()
{

	
	vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	float centreDistance = distance(texCoord, screenCentre);
	float vignette = smoothstep(0.1, 0.8, centreDistance);
	ppColour = vec4(rgb * (1.0 - vignette), 1);

	

}