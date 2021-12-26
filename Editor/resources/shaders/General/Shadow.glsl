#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout (location = 0) in vec3  a_Position;
layout (location = 4) in mat4  a_Transform;

uniform mat4 u_LightMatrix;

void main()
{
	gl_Position 		= u_LightMatrix * a_Transform *  vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout (location = 0) out vec4 FragColor;


float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depth), 1.0);
    //FragColor = vec4(1,1,1, 1.0);
}