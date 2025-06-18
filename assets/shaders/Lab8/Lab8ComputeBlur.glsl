#version 430
layout(local_size_x = 16, local_size_y = 32) in;

layout(binding = 1, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

uniform vec2 u_imageSize;

//This compute shader will output the image blurred.
void main()
{


	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixel_coords) / vec2(u_imageSize);

	vec4 colorSum = vec4(0.0);
	int sampleCount = 0;
	int radius = 5;

	for (int x = -radius; x <= radius; x++)
	{
	
		for(int y = -radius; y <= radius; y++)
		{
		
			vec2 samplePos = uv + vec2(x, y) / u_imageSize;
			samplePos = clamp(samplePos, vec2(0.0), vec2(1.0));

			colorSum += texture(u_image, samplePos);
			sampleCount++;
		
		}
	
	}

	vec4 blurredColor = colorSum / float(sampleCount);
	imageStore(outputImg, pixel_coords, blurredColor);

}