#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;

//This is the fragment shader which outputs a sepia tone to the screen's view.
//Using the dot product and values provided in the labs, it sets a new
//rgb value for the sepia tone, taking the original texture value from rgb.

void main()
{

	
	vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	
	vec3 sepia;
	sepia.r = dot(rgb, vec3(0.393, 0.769, 0.189));
	sepia.g = dot(rgb, vec3(0.349, 0.686, 0.168));
	sepia.b = dot(rgb, vec3(0.272, 0.534, 0.131));

	ppColour = vec4(sepia, 1);

	

}