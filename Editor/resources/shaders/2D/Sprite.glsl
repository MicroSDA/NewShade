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
layout (location = 0) in vec2  a_UV_Coordinates;
layout (location = 0) out vec4 FrameBuffer;
// Textures
layout (binding = 0) uniform sampler2D u_Texture;
void main()
{
	FrameBuffer = texture(u_Texture, a_UV_Coordinates).rgba;
}