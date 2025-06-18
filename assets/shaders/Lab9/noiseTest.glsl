#version 430
//The layout below calculates the grid size of 512 / 512.
layout(local_size_x = 16, local_size_y = 32) in;

layout(binding = 0, rgba8) uniform image2D outputImg;

uniform float octaves;
uniform float frequency;
uniform float amplitude;
uniform float lacunarity;
uniform float persistence;

float remap(float currValue, float  inMin, float inMax, float outMin, float outMax);
vec2 hash2(vec2 p);
float noise(in vec2 p);
float FBM(vec2 position, int octaves);


//I create noise using the functions provided on learningzone.
//I output the image and pixel_coords to an imageStore, to be used in my tessellation shaders in lab7.
void main()
{

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixel_coords) / imageSize(outputImg);

	//float p = noise(uv * 10.0) * 1.5;

	//p = remap(p, -1, 1, 0, 1);
	float p = FBM((uv * 10) * 1.5, int(octaves));

	p = remap(p, -1, 1, 0, 1);

	imageStore(outputImg, pixel_coords, vec4(vec3(p), 1.0));

}

float remap(float currValue, float  inMin, float inMax, float outMin, float outMax) {

	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}

vec2 hash2(vec2 p)
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(dot(hash2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
		dot(hash2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
		mix(dot(hash2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
			dot(hash2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

//Calculating fractal brownian motion.
//Amplitude, frequency, persistence, lacunarity and octaves can be manipulated in the GUI, however,
//changing the amplitude value does not appear to provide a visible change.
float FBM(vec2 position, int octaves)
{

	float total = 0.0f;
	float totalAmplitude = 0.0f;

	float freq = frequency;
	float amp = amplitude;

	for(int i = 0; i < octaves; i++)
	{
	
		total += noise(position * freq) * amp;
		freq *= lacunarity;
		amp *= persistence;
		totalAmplitude += amp;
	
	}
	return (total / totalAmplitude);
}