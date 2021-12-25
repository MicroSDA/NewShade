#type compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout (          binding = 0) uniform          sampler2D  u_InputSampler;
layout (rgba16f,	binding = 1) uniform restrict image2D    u_InputImage;
layout (rgba16f, 	binding = 2) uniform restrict image2D    u_OutputImage;

struct Bloom
{
   vec3  Curve;
   float Exposure;
   float Threshold; 
   float Knee;
};
uniform Bloom	u_Bloom;
// Include our filters
#include "resources/shaders/PostProcess/Bloom/Filters.glsl"

 /* Geting uv coordinates */
vec2 GetUV(sampler2D texture, ivec2 position, int lod)
{
     // 0.5 is offset
     return vec2(vec2(vec2(position.xy) + 0.5) / vec2(textureSize(texture, lod).xy));
}
/* Subroutines declaration */
subroutine void Stage(int lod);
subroutine (Stage)
void HDR(int lod)
{ 
   ivec2 position = ivec2(gl_GlobalInvocationID.xy);
   // Getting hdr color
   float clampValue = 20.0f;
   vec4 color = imageLoad(u_InputImage, position);
   color = min(vec4(clampValue), color);
   color = QThreshold(color, u_Bloom.Curve, u_Bloom.Threshold, u_Bloom.Exposure);

   imageStore(u_OutputImage, position, color);
};
subroutine (Stage)
void Downsample(int lod)
{
   ivec2 position  = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv       = GetUV(u_InputSampler, position / 2, lod + 1);
   vec2  texel     = 1.0 / textureSize(u_InputSampler, lod).xy;
   vec4  color     = DownsampleBox13(u_InputSampler, uv, texel, lod);
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

   vec4 first         = UpsampleTent(u_InputSampler, uv, texel, lod);
   vec4 second        = textureLod(u_InputSampler,   uv,        lod- 1); 

   imageStore(u_OutputImage, position, vec4(first.rgb + second.rgb, first.a));
  
};
subroutine (Stage)
void Combine(int lod)
{
     ivec2 position = ivec2(gl_GlobalInvocationID.xy);
     vec4 first     = imageLoad(u_InputImage, position);
     vec4 second    = imageLoad(u_OutputImage,position);

     vec3 color = first.rgb + second.rgb;
	  //color.rgb  = pow(color.rgb, vec3(1.0 / 1.5));
     imageStore(u_OutputImage, position, vec4(color, first.a));
     //imageStore(u_OutputImage, position, first + second);
};

subroutine uniform Stage s_Stage;
/* Texture mip level */
uniform int	u_Lod;
void main()
{
	s_Stage(u_Lod);
}
