#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];

out vec3 fragmentPos;
out vec2 texCoord;
out vec3 vertexNormal;

vec3 getNormal();

void main()
{

	vec3 geoNormal = getNormal();

	for(int i = 0; i < 3; i++)
	{
	
		texCoord = vs_texCoord[i];
		fragmentPos = vs_fragmentPos[i];
		vertexNormal = geoNormal;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	
	}

	EndPrimitive();

}

vec3 getNormal()
{

	vec3 a = vs_fragmentPos[0] - vs_fragmentPos[1];
	vec3 b = vs_fragmentPos[0] - vs_fragmentPos[2];
	return normalize(cross(a,b));

}