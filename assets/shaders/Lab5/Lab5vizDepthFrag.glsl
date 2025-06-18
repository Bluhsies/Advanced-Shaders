#version 460 core

layout(location = 0) out vec4 depthColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;
uniform float near;
uniform float far;

float LineariseDepth(float depth)
{

	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));

}

void main()
{

	float depthValue = texture(u_originalTexture, texCoord).r;
	float linearDepth = LineariseDepth(depthValue);
	float depthNormalised = linearDepth / far;
	depthColour = vec4(vec3(depthNormalised), 1.0);

}