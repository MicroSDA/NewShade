#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in   vec3  a_Position;
layout (location = 4) in   mat4  a_Transform;
// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};

void main()
{
	gl_Position = u_Camera.ViewProjection  * a_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core
layout (location = 0) out vec4 FrameBuffer;

void main()
{
	//gl_FragDepth = 0;
	FrameBuffer = vec4(0.8, 0.6, 0.2, 0.8);
}