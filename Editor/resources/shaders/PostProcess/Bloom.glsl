#type compute
#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;

layout (rgba8, binding = 0) uniform image2D u_InputColor;
layout (rgba8, binding = 1) uniform image2D u_OutputColor;

struct GaussianData
{
	int				Radius;
	float			Sigma;
	float			Coefficient[7 + 1];
};

layout (std140, binding = 2) uniform UGaussianData
{
	GaussianData u_Gaussian;
};

vec4 CalculateBlur(vec4 color, bool isHorizontal)
{

    float weight[7] = float[] (0.327027, 0.2945946, 0.2216216, 0.064054, 0.026216, 0.016216, 0.000216);
    //float weight[5] = float[] (0.06136, 0.24477, 0.38774, 0.24477,0.06136);
    vec3 result = color.rgb * weight[0];// current fragment's contribution

	if(isHorizontal)
    {
        for(int i = 1; i < 7; ++i)
        {
            result += imageLoad(u_InputColor, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, 0)).rgb * weight[i];
            result += imageLoad(u_InputColor, ivec2(gl_GlobalInvocationID.xy) - ivec2(i, 0)).rgb * weight[i];
        }
    }
    else
    {
 		for(int i = 1; i < 7; ++i)
        {
            result += imageLoad(u_InputColor, ivec2(gl_GlobalInvocationID.xy) + ivec2(0, i)).rgb * weight[i];
            result += imageLoad(u_InputColor, ivec2(gl_GlobalInvocationID.xy) - ivec2(0, i)).rgb * weight[i];
        }
    }

    return vec4(result, color.a);
};

subroutine vec4 Blur(vec4 color);
subroutine (Blur) 
vec4 Horizontal(vec4 color)
{
	return CalculateBlur(color, true);
};
subroutine (Blur) 
vec4 Vertical(vec4 color)
{
	return CalculateBlur(color, false);
};

subroutine uniform Blur  u_Blur;

subroutine vec4 Stage(vec4 color);
subroutine (Stage) 
vec4 GetHDR(vec4 color)
{
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	//return vec4(color.rgb * brightness, 1.0);

  	if(brightness < 0.7)
    	return vec4(0.0, 0.0, 0.0, color.a);
    else return vec4(color.rgb * brightness, color.a);
};
subroutine (Stage) 
vec4 ProcessBlur(vec4 color)
{	
	return u_Blur(color);
};
subroutine (Stage) 
vec4 Combine(vec4 color)
{	
	vec4 first  = color;
	vec4 second = imageLoad(u_OutputColor, ivec2(gl_GlobalInvocationID.xy)).rgba;
	return vec4(second + first);
	//return vec4(second);
};

subroutine uniform Stage u_Stage;
void main()
{

  ivec2 Coordinates = ivec2(gl_GlobalInvocationID.xy);
  vec4  Data        = imageLoad(u_InputColor, Coordinates).rgba;

  imageStore(u_OutputColor, Coordinates, u_Stage(Data));
}