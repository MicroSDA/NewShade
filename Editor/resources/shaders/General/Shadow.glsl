#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout (location = 0) in vec3  a_Position;
layout (location = 4) in mat4  a_Transform;

void main()
{
	gl_Position = a_Transform * vec4(a_Position, 1.0);
}

#type geometry
#version 460 core
#include "resources/shaders/General/Structures.glsl"

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;
    
layout (std430, binding = 5) restrict readonly buffer UDirectlightCascade
{
	DirectLightCascade u_DirectLightCascade[];
};
    
void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = u_DirectLightCascade[gl_InvocationID].ViewMatrix * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
/*layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

layout (std430, binding = 5) restrict readonly buffer UDirectlightCascade
{
	DirectLightCascade u_DirectLightCascade[];
};

void main()
{   
        for(int j = 0; j < 4; j++)
        {
            for(int i= 0; i < gl_in.length();++i)
            {
                gl_Position = u_DirectLightCascade[j].ViewMatrix * gl_in[i].gl_Position;
		        gl_Layer = j;
		        EmitVertex();  
            }
            EndPrimitive();
        }
}*/  

#type fragment
#version 460 core
//layout (location = 0) out vec4 FragColor;


/*float near = 0.01; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}*/

void main()
{

}