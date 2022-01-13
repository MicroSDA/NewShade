#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in  vec3  a_Position;
layout (location = 4) in  mat4  a_Transform;
// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};

layout (std430, binding = 6) restrict readonly buffer USpotLightCascade
{
	SpotLightCascade u_SpotLightCascade[];
};

void main()
{
	/* For camera */
	// vec4 Position 	= a_Transform *  vec4(a_Position.xyz, 1.0);
	// gl_Position 	= u_Camera.ViewProjection * vec4(Position.xyz / Position.w, 1.0);
	/* For spot light */
    vec4 Position 	=  vec4(a_Position.xyz, 1.0);
	gl_Position 	= vec4(Position.xyz / Position.w, 1.0);
}

#type geometry
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout(lines) in;
layout(line_strip, max_vertices = 204) out; // Hardware limitation reached, can only emit 204 vertices of this size

// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};

layout (std430, binding = 6) restrict readonly buffer USpotLightCascade
{
	SpotLightCascade u_SpotLightCascade[];
};

void main()
{    
   for(int j = 0; j < u_SpotLightCascade.length(); j++)
   {
        for(int i= 0; i < gl_in.length(); ++i)
        {
            gl_Position =  u_Camera.ViewProjection * inverse(u_SpotLightCascade[j].ViewMatrix) * gl_in[i].gl_Position;
		    EmitVertex();  
        }
        EndPrimitive();
   }
}
#type fragment
#version 460 core

layout (location = 0)  out vec4 FrameBuffer;

void main()
{
	FrameBuffer = vec4(0.3, 0.8, 0.3, 1.0);
}