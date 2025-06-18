#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform float u_brightness;
uniform vec2 u_screenSize;
uniform vec3 u_Colour;

//This was my attempt a making a lens flare post-processing shader. However, rather than the rays
//coming from one spot and shining at the camera, they look more like multple god rays coming from the sky.

void main()
{

	vec3 rgb = texture(u_originalTexture, texCoord).rgb;

	//Arbitrary values which seemed to fit nicely for the brightness level.
	float brightnessValue = dot(rgb, vec3(0.299, 0.587, 0.114));
	vec3 flare = step(u_brightness, brightnessValue) * rgb;

	vec3 scatterFlare = vec3(0.0);
	vec2 offSet = 1.0 / u_screenSize;

	//iterate through and update the value of scatterflare, based on the original screen texture
	//and assign the offset to it to produce different rays.
	for(int i = -4; i <= 4; i++)
	{
	
		scatterFlare += texture(u_originalTexture, texCoord + vec2(i) * offSet).rgb;

	}

	scatterFlare /= 9.0;

	flare += scatterFlare * u_Colour;

	ppColour = vec4(rgb + flare, 1.0);
	

}