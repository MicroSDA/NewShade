/* Instanced !*/
#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

/* Ve need only positions and transformations */
layout (location = 0) in vec3  a_Position;
layout (location = 4) in mat4  a_Transform;

void main()
{
    /* Set position without veiw matrix */
	gl_Position = a_Transform * vec4(a_Position, 1.0);
}
/* !End of vertex shader */

#type geometry
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;
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

layout (binding = 4) uniform sampler2D u_TShadowMap;

out vec4 FragColor;

void main()
{
    float depthValue = texture(u_TShadowMap, vec2(0.0,0.0)).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue) / 1000), 1.0); // perspective
    // TODO: check alpha for shadow !
}