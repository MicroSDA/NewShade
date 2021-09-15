#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in    vec3 a_Position;
// Camera uniform buffer
layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};
// Has to be inversed
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Camera.Projection * u_Camera.View * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core
layout (location = 0) out vec4 FrameBuffer;
layout (location = 1) out vec4 FrameBuffer2;

void main()
{
	FrameBuffer = vec4(0.3, 0.7, 0.3, 1.0);
}