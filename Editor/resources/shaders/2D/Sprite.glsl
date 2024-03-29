#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_UV_Coordinates;
// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};
// Has to be inversed
uniform mat4 u_Transform;

layout (location = 0) out vec2 out_UV_Coordinates;
void main()
{
	gl_Position = u_Transform * vec4(a_Position.xy, 0.0, 1.0);
	out_UV_Coordinates = a_UV_Coordinates;
}

#type fragment
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout (location = 0) in vec2  a_UV_Coordinates;
layout (location = 0) out vec4 FrameBuffer;

layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};
// Textures
layout (binding = 0) uniform sampler2DArray u_Texture;
//layout (binding = 0) uniform sampler2D u_Texture;

float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{
	vec3 color = vec3(0, 0, 0);
	color += vec3(texture(u_Texture, vec3(a_UV_Coordinates, 3)).r)/ 0.75;
	//color += vec3(texture(u_Texture, vec3(a_UV_Coordinates, 1)).r)/ 0.75;
	//color += vec3(texture(u_Texture, vec3(a_UV_Coordinates, 2)).r)/ 0.75;
	//color += vec3(texture(u_Texture, vec3(a_UV_Coordinates, 3)).r)/ 0.75;
	
	FrameBuffer = vec4(color, 0.0); // orthographic;
	//FrameBuffer = texture(u_Texture, vec3(a_UV_Coordinates, 0)).rgba;
}