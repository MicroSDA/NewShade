/* Instanced !*/
#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

/* Ve need only positions and transformations */
layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec2  a_UV_Coordinates;
layout (location = 4) in mat4  a_Transform;

layout (std430, binding = 7) restrict readonly buffer USpotLightCascade
{
	SpotLightCascade u_SpotLightCascade[];
};

void main()
{
    /* Set position without veiw matrix */
	gl_Position = a_Transform * vec4(a_Position, 1.0);
}
/* !End of vertex shader */

#type geometry
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 204) out; // Hardware limitation reached, can only emit 204 vertices of this size

layout (std430, binding = 7) restrict readonly buffer USpotLightCascade
{
	SpotLightCascade u_SpotLightCascade[];
};

void main()
{    
   for(int j = 0; j < u_SpotLightCascade.length(); j++)
   {
        for(int i= 0; i < gl_in.length();++i)
        {
            gl_Position = u_SpotLightCascade[j].ViewMatrix * gl_in[i].gl_Position;
		    gl_Layer    = j;
		    EmitVertex();  
        }
        EndPrimitive();
    }
}

#type fragment
#version 460 core

layout (location = 0) in vec2  a_UV_Coordinates;

float LinearizeDepth(in vec2 uv)
{
    float zNear = 1.0;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 100.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = gl_FragCoord.z;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

//layout (location = 0) out vec4 FragColor;

void main()
{
    // float c = LinearizeDepth(a_UV_Coordinates);
    // FragColor = vec4(c, c, c, 1.0); // perspective
}