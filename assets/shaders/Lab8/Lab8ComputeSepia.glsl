#version 430
layout(local_size_x = 16, local_size_y = 32) in;

layout(binding = 1, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

uniform vec2 u_imageSize;

//This compute shader will output the image sepia toned.
void main()
{


	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixel_coords) / vec2(u_imageSize);

	vec3 colour = texture(u_image, uv).rgb;

	vec3 sepia;
	sepia.r = dot(colour, vec3(0.393, 0.769, 0.189));
	sepia.g = dot(colour, vec3(0.349, 0.686, 0.168));
	sepia.b = dot(colour, vec3(0.272, 0.534, 0.131));
	
	imageStore(outputImg, pixel_coords, vec4(sepia,1.0));

}