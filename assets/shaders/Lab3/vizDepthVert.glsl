#version 460 core

layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec2 a_texCoord;

out vec2 texCoord;

layout(std140, binding = 2) uniform b_camera2D
{

	uniform mat4 u_view;
	uniform mat4 u_projection;

};

uniform mat4 u_model;

void main()
{

	texCoord = a_texCoord;

	gl_Position = u_projection * u_view * u_model * vec4(a_vertexPosition, 1.0);

}