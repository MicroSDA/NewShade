#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in  vec3 a_Position;
// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};
// Has to be inversed
uniform mat4 u_Transform;

void main()
{
    vec4 Position 	= u_Transform * vec4(a_Position.xyz, 1.0);
	gl_Position 	= u_Camera.ViewProjection * vec4(Position.xyz / Position.w, 1.0);
}

#type fragment
#version 460 core

layout (location = 0)  out vec4 FrameBuffer;

void main()
{
	FrameBuffer = vec4(0.612, 0.098, 0.220, 1.0);
}