#type compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout (            binding = 0) uniform          sampler2D  u_InputSampler;
layout (rgba16f,	binding = 1) uniform restrict image2D    u_InputImage;
layout (rgba16f, 	binding = 2) uniform restrict image2D    u_OutputImage;

#define MAX_GAUSSIAN_RADIUS 8

struct Data
{
	vec3  Curve;
	float Threshold;
	int   BlurRadius;
	float Kernels[MAX_GAUSSIAN_RADIUS];
};

uniform Data u_Data;
// Include our filters
#include "resources/shaders/PostProcess/Bloom/Filters.glsl"

 /* Geting uv coordinates */
vec2 GetUV(sampler2D texture, ivec2 position, int lod)
{
     // 0.5 is offset
     return vec2(vec2(vec2(position.xy) + 0.5) / vec2(textureSize(texture, lod).xy));
}
/* Basic gaussian blur */
vec4 BlurH_V(sampler2D texture, vec2 uv, vec2 texel, int lod)
{
    //float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec4 result = textureLod(texture, uv, lod);
	result.rgb = result.rgb * u_Data.Kernels[0];

    if(lod % 2 == 0)
    {	
		// Horizontal
        for(int i = 1; i < u_Data.BlurRadius; i++)
		{
			result.rgb += textureLod(texture, uv + vec2(texel.x * i, 0.0), lod).rgb * u_Data.Kernels[i];
		    result.rgb += textureLod(texture, uv - vec2(texel.x * i, 0.0), lod).rgb * u_Data.Kernels[i];
		}
    }
    else
    {
		// Vertical
       for(int i = 1; i < u_Data.BlurRadius; i++)
		{
			result.rgb += textureLod(texture, uv + vec2(0.0, texel.y * i), lod).rgb * u_Data.Kernels[i];
		    result.rgb += textureLod(texture, uv - vec2(0.0, texel.y * i), lod).rgb * u_Data.Kernels[i];
		}  
    }

    return vec4(result.rgb, result.a);
}
/* Subroutines declaration */
subroutine void Stage(int lod);
subroutine (Stage)
void HDR(int lod)
{ 
   ivec2 position = ivec2(gl_GlobalInvocationID.xy);
   // Getting hdr color
   vec4 color     = QThreshold(imageLoad(u_InputImage, position), u_Data.Curve, u_Data.Threshold, 5.0);

   imageStore(u_OutputImage, position, color);
};
subroutine (Stage)
void Downsample(int lod)
{
   ivec2 position  = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv       = GetUV(u_InputSampler, position / 2, lod + 1);
   vec2  texel     = 1.0 / textureSize(u_InputSampler, lod).xy;
   vec4  color     = DownsampleBox4(u_InputSampler, uv, texel, lod);
   // position / 2 is size of next mip level
   imageStore(u_OutputImage, position / 2, color);
};
subroutine (Stage)
void Upsample(int lod)
{
   ivec2 position     = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv           = GetUV(u_InputSampler, position, lod - 1); // lod above current
   vec2  uv2          = GetUV(u_InputSampler, position / 2, lod);
   vec2  texel        = 1.0 / textureSize(u_InputSampler, lod).xy;

   vec4 first         = UpsampleBox4(u_InputSampler, uv, texel, lod);
   vec4 second        = textureLod(u_InputSampler,   uv,        lod- 1); 

   
   imageStore(u_OutputImage, position, vec4(first.rgb + second.rgb, first.a));
  
};
subroutine (Stage)
void Blur(int lod)
{
   ivec2 position  = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv        = GetUV(u_InputSampler, position,   lod);
   vec2  texel     = 1.0 / textureSize(u_InputSampler, lod).xy;
   imageStore(u_OutputImage, position / 2,  BlurH_V(u_InputSampler, uv, texel, lod));
};

vec3 ACES(vec3 x)
 {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
  //return x;
}

subroutine (Stage)
void Combine(int lod)
{
     ivec2 position = ivec2(gl_GlobalInvocationID.xy);
     vec4 first     = imageLoad(u_InputImage, position);
     vec4 second    = imageLoad(u_OutputImage,position);

     imageStore(u_OutputImage, position, vec4(ACES(first.rgb + second.rgb), first.a));
};

subroutine uniform Stage s_Stage;
/* Texture mip level */
uniform int	u_Lod;

void main()
{
	s_Stage(u_Lod);
}
