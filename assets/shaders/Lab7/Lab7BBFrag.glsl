#version 460 core

in vec2 texCoord;
out vec4 fragColor;

uniform vec3 u_albedo;
uniform sampler2D u_billboardImage;

void main()
{

	vec4 sampledCol = texture(u_billboardImage, texCoord);

	float luminance = 0.2126 * sampledCol.r + 0.7152 * sampledCol.g + 0.0722 + sampledCol.b;
	vec3 tintedCol = mix(sampledCol.rgb, sampledCol.rgb * u_albedo, luminance);


	if (sampledCol.a  < 0.001) discard;

	fragColor = vec4(tintedCol, sampledCol.a);


}