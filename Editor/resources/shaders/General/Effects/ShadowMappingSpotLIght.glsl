/* Instanced !*/
#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

/* Ve need only positions and transformations */
layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec2  a_UV_Coordinates;
layout (location = 4) in mat4  a_Transform;

layout (location = 0) out vec2  out_UV_Coordinates;

layout (std430, binding = 6) restrict readonly buffer USpotLightCascade
{
	mat4 u_SpotLightCascade[];
};

void main()
{
    /* Set position without veiw matrix */
	gl_Position = u_SpotLightCascade[0] * a_Transform * vec4(a_Position, 1.0);
	out_UV_Coordinates = a_UV_Coordinates;
}
/* !End of vertex shader */

#type fragment
#version 460 core

layout (location = 0) in vec2  a_UV_Coordinates;

float LinearizeDepth(in vec2 uv)
{
    float zNear = 0.5;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 500.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = gl_FragCoord.z;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

layout (location = 0) out vec4 FragColor;

void main()
{
    float c = LinearizeDepth(a_UV_Coordinates);
    FragColor = vec4(c, c, c, 1.0); // perspective
}