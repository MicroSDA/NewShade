#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0) in  vec3 Position;
// Output attributes
layout(location  = 0) out vec3 out_Near;
layout(location  = 1) out vec3 out_Far;
// Camera uniform buffer
layout(std140, binding = 0) uniform UCamera
{
    Camera u_Camera;
};

vec3 UnprojectPoint(float x, float y, float z, mat4 viewProjection) 
{
    vec4    unprojectedPoint    =  inverse(viewProjection) * vec4(x, y, z, 1.0);
    return  unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	out_Near 		= UnprojectPoint(Position.x, Position.y, 0.0, u_Camera.ViewProjection).xyz; // unprojecting on the near plane
	out_Far  		= UnprojectPoint(Position.x, Position.y, 1.0, u_Camera.ViewProjection).xyz; // unprojecting on the far plane
	gl_Position 	= vec4(Position, 1.0);
}

#type fragment
#version 460 core
#include "resources/shaders/General/Structures.glsl"
// Input attributes
layout (location = 0)   in vec3 a_Near; // nearPoint calculated in vertex shader
layout (location = 1)   in vec3 a_Far; // farPoint calculated in vertex shader
// Camera uniform buffer
layout(std140, binding = 0) uniform UCamera
{
    Camera u_Camera;
};

vec4 Grid(vec3 gridColor, vec3 fragPos3D, float scale, bool drawAxis) 
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 0.1);
    float minimumx = min(derivative.x, 0.1);
    vec4 color = vec4(gridColor.rgb, 1.0 - min(line, 1));
    // z axis
    if(fragPos3D.x > - 3 * minimumx && fragPos3D.x < 3 * minimumx)
       color.rgb = vec3(0.0, 0.0, 2.0);
    // x axis
    if(fragPos3D.z > - 3 * minimumz && fragPos3D.z < 3 * minimumz)
        color.rgb = vec3(2.0, 0.0, 0.0);
    return color;
}

float ComputeDepth(vec3 pos) 
{
    vec4 clip_space_pos     = u_Camera.Projection * u_Camera.View * vec4(pos.xyz, 1.0);
    float clip_space_depth  =  (clip_space_pos.z / clip_space_pos.w);
	float far               = gl_DepthRange.far;
	float near              = gl_DepthRange.near;
	float depth             = (((far-near) * clip_space_depth) + near + far) / 2.0;

	return depth;
}

float ComputeLinearDepth(vec3 pos) 
{
    vec4 clip_space_pos     = u_Camera.Projection * u_Camera.View * vec4(pos.xyz, 1.0);
    float clip_space_depth  = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth       = (2.0 * gl_DepthRange.near * gl_DepthRange.far) / (gl_DepthRange.far + gl_DepthRange.near - clip_space_depth * (gl_DepthRange.far - gl_DepthRange.near)); // get linear value between 0.01 and 100
    return linearDepth / 100; // normalize
}

layout (location = 0) out vec4 FrameBuffer;

void main()
{
	float t             = -a_Near.y / (a_Far.y - a_Near.y);
    vec3 fragPos3D      = a_Near + t * (a_Far - a_Near);
    float linearDepth   = ComputeLinearDepth(fragPos3D);
    float fading        = max(0, (0.4 - linearDepth));

    gl_FragDepth        = ComputeDepth(fragPos3D);
    vec3 gridColor      =  vec3(0.2, 0.2, 0.2);
    vec4 Color          = (Grid(gridColor, fragPos3D, 0.1, true) + Grid(gridColor, fragPos3D, 1.0, true))* float(t > 0);
	Color.a      *= fading;
	if(Color.a <= 0.2)
		discard;
    FrameBuffer = Color;
}