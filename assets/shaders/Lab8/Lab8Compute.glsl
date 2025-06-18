#version 430
layout(local_size_x = 16, local_size_y = 32) in;

layout(binding = 0, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

uniform vec2 u_imageSize;

//This compute shader will output the image inverted.
void main()
{


	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixel_coords) / vec2(u_imageSize);

	vec4 color = texture(u_image, uv);

	color.rgb = 1.0 - color.rgb;

	imageStore(outputImg, pixel_coords, color);


}