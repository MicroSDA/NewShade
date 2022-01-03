/* Instanced !*/
#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

/* Ve need only positions and transformations */
layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec2  a_UV_Coordinates;
layout (location = 4) in mat4  a_Transform;

layout (location = 0) out vec2  out_UV_Coordinates;

layout (std430, binding = 6) restrict readonly buffer USpotLightVeiwMatrix
{
	mat4 u_SpotLightVeiwMatrix[];
};

void main()
{
    /* Set position without veiw matrix */
	gl_Position = u_SpotLightVeiwMatrix[0] * a_Transform * vec4(a_Position, 1.0);
	out_UV_Coordinates = a_UV_Coordinates;
}
/* !End of vertex shader */

#type fragment
#version 460 core

layout (location = 0) in vec2  a_UV_Coordinates;

float LinearizeDepth(float depth, float far_plane, float near_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

layout (binding = 4) uniform sampler2D u_TShadowMap;

layout (location = 0) out vec4 FragColor;

void main()
{
    //float depthValue = texture(u_TShadowMap, a_UV_Coordinates).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue, 1000, 0.1), 1.0); // perspective
	//FragColor = vec4(vec3(LinearizeDepth(depthValue, 100, 1)), 1.0); // perspective
	//FragColor = vec4(1, 1, 0, 1.0); // perspective
    // TODO: check alpha for shadow !
}