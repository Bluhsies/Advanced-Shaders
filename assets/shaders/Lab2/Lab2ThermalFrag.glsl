#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;

vec3 thermalGradient(float intensity);

//This was my attempt at a thermal imaging type of post processing.
//Whilst it does not work in a real world sense, as it shows that part of the sky and the hair are
//to be considered the hottest parts, it produces the different colouring effects and with a bit more
//work, perhaps looking at using depth in some way, I could get the skin to be the hottest.

void main()
{

	
	vec4 texColour = texture(u_originalTexture, texCoord);

	float intensity = dot(texColour.rgb, vec3(0.299, 0.587, 0.114)); // Convert to intensity via greyscale luminance

	vec3 thermalColour = thermalGradient(intensity);

	ppColour = vec4(thermalColour, 1.0);
	

}

//Checks the value of intensity for the fragment and compares it with the if statements to output a colour to the screen.
//All start off black and follows the steps until a colour is output.
vec3 thermalGradient(float intensity)
{

	if (intensity < 0.2) return mix(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), intensity / 0.2); // Black to blue colouring
    else if (intensity < 0.4) return mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), (intensity - 0.2) / 0.2); // Blue to green colouring
	else if (intensity < 0.6) return mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (intensity - 0.4) / 0.2); // Green to yellow colouring
    else if (intensity < 0.8) return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (intensity - 0.6) / 0.2); // Yellow to red colouring
    else return mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), (intensity - 0.8) / 0.2); // Red to white colouring

}