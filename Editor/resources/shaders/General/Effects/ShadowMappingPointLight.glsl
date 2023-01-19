/* Instanced !*/
#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

/* Ve need only positions and transformations */
layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec2  a_UV_Coordinates;
layout (location = 4) in mat4  a_Transform;

layout (location = 0) out vec2  out_UV_Coordinates;

void main()
{
    /* Set position without veiw matrix */
	gl_Position = a_Transform * vec4(a_Position, 1.0);
    out_UV_Coordinates = a_UV_Coordinates;
}
/* !End of vertex shader */

#type geometry
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 73) out; // Hardware limitation reached, can only emit 85 vertices of this size

layout (location = 0)  in  vec2  a_UV_Coordinates[];
layout (location = 0)  out vec3  out_FragmentPosition;
layout (location = 1)  out vec3  out_LightPosition;
layout (location = 2)  out float out_Distance;
layout (location = 3)  out vec2  out_UV_Coordinates;
// Point lights SSBO buffer
layout (std430, binding = 3) restrict readonly buffer UPointLight
{
	PointLight u_PointLight[];
};

layout (std430, binding = 9) restrict readonly buffer UPointLightCascade
{
	PointLightCascade u_PointLightCascade[];
};

void main()
{    
    for(int j = 0; j < u_PointLightCascade.length(); j++)
    {
        gl_Layer            = j;
        out_Distance        = u_PointLight[int(j / 6)].Distance;
        out_LightPosition   = u_PointLight[int(j / 6)].Position.xyz;

        for(int i = 0; i < gl_in.length(); i++)
        {
            gl_Position               = u_PointLightCascade[j].ViewMatrix * gl_in[i].gl_Position;
            out_FragmentPosition      = gl_in[i].gl_Position.xyz;
            out_UV_Coordinates = a_UV_Coordinates[i];
            EmitVertex(); 
        }
        EndPrimitive();
    }
}

#type fragment
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout (location = 0)  in vec3  in_FragmentPosition;
layout (location = 1)  in vec3  in_LightPosition;
layout (location = 2)  in float in_Distance;
layout (location = 3)  in vec2  a_UV_Coordinates;

layout (binding = 0) uniform sampler2D 	u_TDiffuse;

void main()
{

float Alpha = texture(u_TDiffuse, a_UV_Coordinates).a;
	if(Alpha == 0.0)
		gl_FragDepth = 1.0;
	else 
	{
        float LightDistance = length(in_FragmentPosition - in_LightPosition);
        gl_FragDepth = LightDistance / in_Distance;
    }
}