#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tes_fragmentPos[];
in vec2 tes_texCoord[];
in vec3 tes_CDMnormal[];
in float tes_scale[];

out vec3 fragmentPos;
out vec2 texCoord;
out vec3 vertexNormal;
out vec3 CDMnormal;
out float scale;

vec3 getNormal();


//This iteration of the floorPhoneGeo calculates the CDMnormals, which are sent to the fragment shader.
void main()
{

	vec3 geoNormal = getNormal();

	for(int i = 0; i < 3; i++)
	{
	
		texCoord = tes_texCoord[i];
		fragmentPos = tes_fragmentPos[i];
		vertexNormal = geoNormal;
		CDMnormal = tes_CDMnormal[i];
		scale = tes_scale[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	
	}

	EndPrimitive();

}

vec3 getNormal()
{

	vec3 a = tes_fragmentPos[0] - tes_fragmentPos[1];
	vec3 b = tes_fragmentPos[0] - tes_fragmentPos[2];
	return normalize(cross(a,b));

}