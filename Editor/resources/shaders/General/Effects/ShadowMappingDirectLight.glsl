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

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

 layout (location = 0) in  vec2  a_UV_Coordinates[];
 layout (location = 0) out vec2  out_UV_Coordinates;

/* Direct light cascades */
layout (std430, binding = 5) restrict readonly buffer UDirectlightCascade
{
	DirectLightCascade u_DirectLightCascade[];
};

void main()
{   

    for (int i = 0; i < 3; ++i)
	{
		gl_Position = u_DirectLightCascade[gl_InvocationID].ViewMatrix * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		//out_UV_Coordinates = a_UV_Coordinates[i];
		EmitVertex();
	}
	EndPrimitive();
}
/* !End of geometry shader*/
#type fragment
#version 460 core

float LinearizeDepth(float depth, float far_plane, float near_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

layout (location = 0) in vec2 a_UV_Coordinates;

layout (binding = 0) uniform sampler2D 	u_TDiffuse;

out vec4 FragColor;

void main()
{
	// float Alpha = texture(u_TDiffuse, a_UV_Coordinates).a;
	// if(Alpha == 0.0)
	// 	gl_FragDepth = 1.0;
	// else 
	// 	gl_FragDepth =gl_FragCoord.z;
}