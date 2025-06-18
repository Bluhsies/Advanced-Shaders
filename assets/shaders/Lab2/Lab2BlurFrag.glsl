#version 460 core

//This is the fragment shader for my blur post-processing.

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform vec2 screenSize;
uniform float blurRadius;

void main()
{

	vec2 texelSize = 1.0 / screenSize;

	vec3 result = vec3(0.0);
	float samplesTaken = 0.0;

	// Iterates through x and y values to produce a colour output which blurs the screen's output.
	// It increases the number of samples taken each iteration and divides the result vec3
	// to produce the output colour.
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
		
			vec2 offset = vec2(float(x), float(y)) * texelSize * blurRadius;
			vec3 sampleCol = texture(u_originalTexture, texCoord + offset).rgb;
			result += sampleCol;
			samplesTaken++;
		}
	}

	result /= samplesTaken;

	ppColour = vec4(result, 1.0);


	

}