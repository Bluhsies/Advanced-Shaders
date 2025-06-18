#version 460 core
//layout(triangles, equal_spacing, ccw) in;
layout(triangles, fractional_even_spacing, ccw) in;
//layout(triangles, fractional_odd_spacing, ccw) in;

in vec3 tcs_fragmentPos[];
in vec2 tcs_texCoord[];

out vec3 tes_fragmentPos;
out vec2 tes_texCoord;
out vec3 tes_CDMnormal;
out float tes_scale;

layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2);
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2);

//I sample the heightmap used from the noise texture.
uniform sampler2D u_heightMap;
uniform mat4 u_modelMatrix;
uniform float u_scale;

//In the main, I calculate the CDM normal and send this to the geometry shader.
//I also scale the fragment position in world space by a scale value.
void main()
{


	tes_scale = u_scale;

	tes_fragmentPos = interpolate3D(tcs_fragmentPos[0], tcs_fragmentPos[1], tcs_fragmentPos[2]);
	tes_texCoord = interpolate2D(tcs_texCoord[0], tcs_texCoord[1], tcs_texCoord[2]);

	float height = texture(u_heightMap, tes_texCoord).r;
	tes_fragmentPos.y = height*u_scale;

	float right = (textureOffset(u_heightMap, tes_texCoord, ivec2(1,0)).r)*u_scale;
	float left = (textureOffset(u_heightMap, tes_texCoord, ivec2(-1,0)).r)*u_scale;
	float up = (textureOffset(u_heightMap, tes_texCoord, ivec2(0,-1)).r)*u_scale;
	float down = (textureOffset(u_heightMap, tes_texCoord, ivec2(0,1)).r)*u_scale;

	float lr = left-right;
	float du = up-down;
	tes_CDMnormal = normalize(vec3(lr,2.0,du));

	gl_Position = u_projection * u_view * u_modelMatrix * vec4(tes_fragmentPos, 1.0);

}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{

	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;

}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{

	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;

}