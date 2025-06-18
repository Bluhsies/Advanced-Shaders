#version 460 core

layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;


// Camera layout
layout(std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

//Data sent to the fragment shader to output the model to the screen.
out vec3 fragmentPos;
out vec3 vertexNormal;
out vec2 texCoord;
out vec4 fragPosClipSpace;
out mat3 TBN;

uniform mat4 u_model;

void main()
{
	fragmentPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	vertexNormal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
	texCoord = a_texCoord;
	//gl_Position = u_projection * u_view * vec4(fragmentPos, 1.0);

	// Calculate the TBN normals, which are passed to the fragment shader.
	vec3 T = (u_model * vec4(a_tangent, 0.0 )).xyz;
	T = normalize(T - dot(T, vertexNormal) * vertexNormal);
	vec3 B = cross(vertexNormal, T);
	B = normalize(B);
	TBN = mat3(T, B, vertexNormal);

	fragPosClipSpace = u_projection * u_view * vec4(fragmentPos, 1.0);

	gl_Position = fragPosClipSpace;

}