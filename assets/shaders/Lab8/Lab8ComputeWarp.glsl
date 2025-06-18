#version 430
layout(local_size_x = 16, local_size_y = 32) in;

layout(binding = 0, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

uniform vec2 u_imageSize;

uniform float u_warp;

//This compute shader will output the image warped and allow the user to change the warp level.
void main()
{


	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(pixel_coords) / u_imageSize * 2.0 - 1.0;
    
    //Converts to polar coordinates
    float radius = length(uv);
    float angle = atan(uv.y, uv.x);

    //Apply warp spiral
    float warpAmount = u_warp * radius;
    angle += warpAmount;

    //Converts back to cartesian coordinates
    vec2 warpedUV = vec2(cos(angle), sin(angle)) * radius;
    warpedUV = (warpedUV + 1.0) / 2.0;

    //Sample the image
    vec3 colour = texture(u_image, warpedUV).rgb;

    // Output the final color
    imageStore(outputImg, pixel_coords, vec4(colour, 1.0));


}