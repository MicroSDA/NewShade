#type vertex
#version 460 core

struct Camera
{
	mat4 _View;
	mat4 _Projection;
	vec3 _Position;
	vec3 _Forward;
};

layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};

layout (location = 0) in  vec3 Position;

uniform mat4 CameraMatrix;

void main()
{
    vec4 pos    = CameraMatrix * vec4(Position.xyz, 1.0);
	gl_Position = u_Camera._Projection * u_Camera._View * vec4(pos.xyz / pos.w, 1.0);
}

#type fragment
#version 460 core

void main()
{
	gl_FragColor = vec4(0, 1, 0, 0.5);
}