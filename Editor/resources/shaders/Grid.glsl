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

layout(location = 1) out vec3 out_Near;
layout(location = 2) out vec3 out_Far;
layout(location = 3) out mat4 out_Veiw;
layout(location = 7) out mat4 out_Projection;

//uniform Camera u_Camera;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	out_Near 		= UnprojectPoint(Position.x, Position.y, 0.0, u_Camera._View, u_Camera._Projection).xyz; // unprojecting on the near plane
	out_Far  		= UnprojectPoint(Position.x, Position.y, 1.0, u_Camera._View, u_Camera._Projection).xyz; // unprojecting on the far plane
	out_Projection  = u_Camera._Projection;
	out_Veiw  		= u_Camera._View;
	gl_Position 	= vec4(Position, 1.0f);
}

#type fragment
#version 460 core

layout (location = 0)   out vec4 ColorAttachment;
layout (location = 1)   in vec3 nearPoint; // nearPoint calculated in vertex shader
layout (location = 2)   in vec3 farPoint; // farPoint calculated in vertex shader
layout (location = 3)   in mat4 fragView;
layout (location = 7)   in mat4 fragProj;


vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1));
    // z axis
    if(fragPos3D.x > - 10 * minimumx && fragPos3D.x < 5 * minimumx)
       color.rgb = vec3(0.0, 0.0, 1.0);
    // x axis
    if(fragPos3D.z > - 10 * minimumz && fragPos3D.z < 5 * minimumz)
        color.rgb = vec3(1.0, 0.0, 0.0);
    return color;
}
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth =  (clip_space_pos.z / clip_space_pos.w);
	float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;

	float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;

	return depth;
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * gl_DepthRange.near * gl_DepthRange.far) / (gl_DepthRange.far + gl_DepthRange.near - clip_space_depth * (gl_DepthRange.far - gl_DepthRange.near)); // get linear value between 0.01 and 100
    return linearDepth / 100; // normalize
}

//layout (depth_any) out float gl_FragDepth;

void main()
{
	
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    ColorAttachment = (grid(fragPos3D, 0.01, true) + grid(fragPos3D, 0.1, true))* float(t > 0); 
	ColorAttachment.a *= fading;
	if(ColorAttachment.a <= 0.2)
		discard;
	
}