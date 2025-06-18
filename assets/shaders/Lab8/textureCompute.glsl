#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba8) uniform image2D outputImg;

void main()
{


	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 pixelCol = vec4(1.0, 0.5, 1.0, 1.0);

	/*if (gl_WorkGroupID.x == 16)
	{

		pixelCol = vec4(0.0, 0.0, 0.5, 1.0);

	}*/

	imageStore(outputImg, pixel_coords, pixelCol);


}