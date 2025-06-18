#version 460 core

layout(location = 0) in vec3 a_vertexPosition;

out vec3 texCoords;

layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

uniform mat4 u_skyboxView;

void main()
{
	//Takes the value of a_vertexPosition and sends it to the fragment shader as texCoords.
	texCoords = a_vertexPosition;
	gl_Position = u_projection * u_skyboxView * vec4(a_vertexPosition, 1.0);
}