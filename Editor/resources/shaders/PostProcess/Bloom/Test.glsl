#type compute
#version 460 core

layout(local_size_x = 16, local_size_y = 16) in;

layout (            binding = 0) uniform          sampler2D  u_InputSampler;
layout (rgba16f,	binding = 1) uniform restrict image2D    u_InputImage;
layout (rgba16f, 	binding = 2) uniform restrict image2D    u_OutputImage; 

#define EPSILON 1.0e-4
vec4 QThreshold(vec4 color, vec3 curve, float threshold, float exposure)
{
	color *= exposure;
	float br =  max(max(color.r, color.g), color.b);
    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    // Combine and apply the brightness response curve.
    color.rgb *= max(rq, br - threshold) / max(br, EPSILON);

    return color;
}


vec4 DownsampleBox4(sampler2D texture, vec2 uv, vec2 texelSize, float lod)
{
	vec4 offset  = texelSize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) ;
	
    vec4 color 	 = textureLod(texture, uv + offset.xy, lod);
    color 		+= textureLod(texture, uv + offset.zy, lod);
    color 		+= textureLod(texture, uv + offset.xw, lod);
    color 		+= textureLod(texture, uv + offset.zw, lod);

    return  color * (1.0 / 4.0);
}

vec4 DownsampleBox13(sampler2D texture, vec2 uv, vec2 texelSize, float lod)
{
	
	vec4 A = textureLod(texture, uv + texelSize * vec2(-1.0, -1.0), lod);
    vec4 B = textureLod(texture, uv + texelSize * vec2( 0.0,  0.0), lod);
    vec4 C = textureLod(texture, uv + texelSize * vec2( 1.0, -1.0), lod);
    vec4 D = textureLod(texture, uv + texelSize * vec2(-0.5, -0.5), lod);
    vec4 E = textureLod(texture, uv + texelSize * vec2( 0.5, -0.5), lod);
    vec4 F = textureLod(texture, uv + texelSize * vec2(-1.0,  0.0), lod);

    vec4 G = textureLod(texture, uv, lod				   	             );

    vec4 H = textureLod(texture, uv + texelSize * vec2( 1.0,  0.0), lod);
    vec4 I = textureLod(texture, uv + texelSize * vec2(-0.5,  0.5), lod);
    vec4 J = textureLod(texture, uv + texelSize * vec2( 0.5,  0.5), lod);
    vec4 K = textureLod(texture, uv + texelSize * vec2(-1.0,  1.0), lod);
    vec4 L = textureLod(texture, uv + texelSize * vec2( 0.0,  1.0), lod);
    vec4 M = textureLod(texture, uv + texelSize * vec2( 1.0,  1.0), lod);

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 color  = (D + E + I + J) * div.x;
		 color += (A + B + G + F) * div.y;
    	 color += (B + C + H + G) * div.y;
    	 color += (F + G + L + K) * div.y;
    	 color += (G + H + M + L) * div.y;

    return color;
}

vec4 UpsampleBox4(sampler2D texture, vec2 uv, vec2 texelSize, float lod)
{
	vec4 offset  = texelSize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) * (2.0 * 0.5);
	
    vec4 color 	 = textureLod(texture, uv + offset.xy, lod);
    color 		+= textureLod(texture, uv + offset.zy, lod);
    color 		+= textureLod(texture, uv + offset.xw, lod);
    color 		+= textureLod(texture, uv + offset.zw, lod);

    return  color * (1.0 / 4.0);
}

vec4 UpsampleTent(sampler2D texture, vec2 uv, vec2 texelSize, float lod)
{
	vec4 offset  = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 1.0) * -(1);
	
	vec4 color   = textureLod(texture, uv - offset.xy, lod);
	color 		+= textureLod(texture, uv - offset.wy, lod) * 2.0;
    color 		+= textureLod(texture, uv - offset.zy, lod);
	
    color 		+= textureLod(texture, uv + offset.zw, lod) * 2.0;
	color 		+= textureLod(texture, uv 		     , lod) * 4.0;
	color 		+= textureLod(texture, uv + offset.xw, lod) * 2.0;
		
	color 		+= textureLod(texture, uv + offset.zy, lod);
	color 		+= textureLod(texture, uv + offset.wy, lod) * 2.0;
	color 		+= textureLod(texture, uv + offset.xy, lod);
	
	return color * (1.0 / 16.0);
}
vec2 GetUV(sampler2D texture, ivec2 position, int lod)
{
     // 0.5 is offset
     return vec2(vec2(vec2(position.xy) + 0.5) / vec2(textureSize(texture, lod).xy));
}

vec3 BlurHV(sampler2D texture, vec2 uv, vec2 texel, int lod)
{
    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec3 result = textureLod(texture, uv, lod).rgb * weight[0];

    if(lod % 2 == 0)
    {
        for(int i = 1; i < 5; i++)
		{
			result += textureLod(texture, uv + vec2(texel.x * i, 0.0), lod).rgb * weight[i];
		    result += textureLod(texture, uv - vec2(texel.x * i, 0.0), lod).rgb * weight[i];
		}
    }
    else
    {
      for(int i = 1; i < 5; i++)
		{
			result += textureLod(texture, uv + vec2(0.0, texel.y * i), lod).rgb * weight[i];
		    result += textureLod(texture, uv - vec2(0.0, texel.y * i), lod).rgb * weight[i];
		}  
    }

    return result;
}
subroutine void Stage(int lod);
subroutine (Stage)
void HDR(int lod)
{                   // Related to x, y component execution
   ivec2 position = ivec2(gl_GlobalInvocationID.xy);
   vec4 color     = QThreshold(imageLoad(u_InputImage, position), vec3(0.2126, 0.7152, 0.0722), 0.5, 3.0);

   imageStore(u_OutputImage, position, color);
}

subroutine (Stage)
void Down(int lod)
{
   ivec2 position  = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv        = GetUV(u_InputSampler, position, lod);
   //vec4  color     = textureLod(u_InputSampler, uv, lod);
   vec2  texel     = 1.0 / textureSize(u_InputSampler, lod).xy;
   vec4  color     = DownsampleBox4(u_InputSampler, uv, texel, lod);
   
   imageStore(u_OutputImage, position / 2, color);
}

subroutine (Stage)
void Up(int lod)
{
   ivec2 position = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv       = GetUV(u_InputSampler, position / 2, lod);
   vec2  uv2      = GetUV(u_InputSampler, position,     lod-1);
   vec2  texel1     = 1.0 / textureSize(u_InputSampler, lod).xy;
   vec2  texel2     = 1.0 / textureSize(u_InputSampler, lod-1).xy;
   //vec4 first     = textureLod(u_InputSampler,  uv,     lod);
   vec4 first     = UpsampleBox4(u_InputSampler,  uv2,    texel1, lod); // если тут uv2 то фиксит смещение
   vec4 second    = textureLod(u_InputSampler,    uv2,    lod     - 1);
   //vec4 second    = UpsampleBox4(u_InputSampler,  uv2,  texel2, lod -1);
   imageStore(u_OutputImage, position, first + second);
}
subroutine (Stage)
void Blur(int lod)
{
   ivec2 position  = ivec2(gl_GlobalInvocationID.xy);
   vec2  uv        = GetUV(u_InputSampler, position,   lod);
   vec2  texel     = 1.0 / textureSize(u_InputSampler, lod).xy;
   //vec4  color     = textureLod(u_InputSampler, uv, lod);
   imageStore(u_OutputImage, position / 2,  vec4(BlurHV(u_InputSampler, uv, texel, lod), 1.0));
}
subroutine (Stage)
void Combine(int lod)
{
     ivec2 position = ivec2(gl_GlobalInvocationID.xy);
     vec4 first     = imageLoad(u_InputImage, position);
     vec4 second    = imageLoad(u_OutputImage,position);

     imageStore(u_OutputImage, position, first + second);
}

subroutine uniform Stage s_Stage;

uniform int u_Lod;
void main()
{
	s_Stage(u_Lod);
}