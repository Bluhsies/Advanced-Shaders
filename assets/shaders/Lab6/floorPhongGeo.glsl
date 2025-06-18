#version 460 core

//Set the layout to send out triangles.
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//Geometry shader takes things in as an array as it holds the gl_Position of each vertex.
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
		EmitVertex(); //outputs the vertex to the scene each iteration.
	
	}

	EndPrimitive(); //Finished with the current primitive.

}

//Calculates the normal using the fragment positions of the triangle, sent from the vertex shader.
vec3 getNormal()
{

	vec3 a = vs_fragmentPos[0] - vs_fragmentPos[1];
	vec3 b = vs_fragmentPos[0] - vs_fragmentPos[2];
	return normalize(cross(a,b));

}