#version 460 core
layout (vertices = 3) out;

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];

out vec3 tcs_fragmentPos[];
out vec2 tcs_texCoord[];

layout (std140, binding = 0) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

//uniform float u_tess;

float GetTessLevel(float Distance0, float Distance1);

void main()
{

	//float tessLevel = u_tess;

	float eyeToVertexDist0 = distance(u_viewPos, vs_fragmentPos[0]);
	float eyeToVertexDist1 = distance(u_viewPos, vs_fragmentPos[1]);
	float eyeToVertexDist2 = distance(u_viewPos, vs_fragmentPos[2]);


	if(gl_InvocationID==0)
	{
		gl_TessLevelOuter[0] = GetTessLevel(eyeToVertexDist1, eyeToVertexDist2);
		gl_TessLevelOuter[1] = GetTessLevel(eyeToVertexDist2, eyeToVertexDist0);
		gl_TessLevelOuter[2] = GetTessLevel(eyeToVertexDist0, eyeToVertexDist1);
		gl_TessLevelInner[0] = gl_TessLevelOuter[2];
	}


	tcs_fragmentPos[gl_InvocationID] = vs_fragmentPos[gl_InvocationID];
	tcs_texCoord[gl_InvocationID] = vs_texCoord[gl_InvocationID];

}

//Here we calculate the tessellation level. I have found that exponential decay provides the best outcome.
float GetTessLevel(float Distance0, float Distance1)
{


	float AvgDistance = (Distance0 + Distance1) / 2.0;

	// Linear
	/*float a = 0.3061;
	float b = 16.3061;

	float outputTess = clamp(-a * AvgDistance + b, 1.0, 64.0);
	return outputTess;*/

	//Step with if statements
	/*if(AvgDistance < 5.0)
	{
	
		return 64.0;

	}
	else if(AvgDistance < 10.0)
	{
	
		return 32.0;

	}
	else if(AvgDistance < 20.0)
	{
	
		return 16.0;

	}
	else if(AvgDistance < 40.0)
	{
	
		return 8.0;

	}
	else
	{
	
		return 1.0;

	}*/

	//Exponential Decay
	float maxTessLevel = 64.0;
	float minTessLevel = 1.0;
	float decayLevel = 0.1;

	float outputTess = maxTessLevel * exp(-decayLevel * AvgDistance);

	outputTess = clamp(outputTess, minTessLevel, maxTessLevel);

	return outputTess;

}