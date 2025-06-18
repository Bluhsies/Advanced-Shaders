#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_originalTexture;

const float gamma = 2.2;

vec3 aces(vec3 x)
{

	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;

	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{

	//vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	//ppColour = vec4(rgb, 1);

	//Reinhard tone mapping
	/*vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	vec3 reinMap = rgb / (rgb + vec3(1.0));
	reinMap = pow(reinMap, vec3(1.0 / gamma));
	ppColour = vec4(reinMap, 1);*/

	//Aces tone mapping
	vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	vec3 acesMap = aces(rgb);
	acesMap = pow(acesMap, vec3(1.0 / gamma));
	ppColour = vec4(acesMap, 1);

	//uncharted2 tone mapping
	/*vec3 rgb = texture(u_originalTexture, texCoord).rgb;
	vec3 unchartedMap = uncharted2_tonemap_partial(rgb);
	unchartedMap = pow(unchartedMap, vec3(1.0 / gamma));
	ppColour = vec4(unchartedMap, 1);*/

}