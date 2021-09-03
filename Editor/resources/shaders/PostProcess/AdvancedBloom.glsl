#type compute
#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;

layout (rgba8,   binding = 0) uniform image2D u_InputColorI;
layout (rgba8,   binding = 1) uniform image2D u_OutputColorI;

layout (rgba16f, binding = 2) uniform image2D u_InputColor;
layout (rgba16f, binding = 3) uniform image2D u_OutputColor;

struct GaussianData
{
	float	Kernels[8];
	int		Radius;
};

uniform GaussianData u_Gaussian;

subroutine void Stage();
subroutine (Stage) 
void HDR()
{
	vec4  input_image  = imageLoad(u_InputColorI, ivec2(gl_GlobalInvocationID.xy));
	float brightness   = dot(input_image.rgb, vec3(0.2126, 0.7152, 0.0722));
	
  	if(brightness < 0.8)
		imageStore(u_InputColor, ivec2(gl_GlobalInvocationID.xy), vec4(0.0, 0.0, 0.0, input_image.a));
	else
		imageStore(u_InputColor, ivec2(gl_GlobalInvocationID.xy), vec4(input_image.rgb * brightness, input_image.a));
	
};

subroutine (Stage)
void BlurH()
{
	ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);

    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	vec4 result = imageLoad(u_OutputColor, texCoords).rgba;
	result.rgb  = result.rgb * weight[0];

	for(int j = 1; j < 5; j++)
	{
		result.rgb += imageLoad(u_OutputColor, texCoords + ivec2(j, 0)).rgb * weight[j];
		result.rgb += imageLoad(u_OutputColor, texCoords - ivec2(j, 0)).rgb * weight[j];
	}
	
	imageStore(u_OutputColor, texCoords, result);
};

subroutine (Stage)
void BlurV()
{
	ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);

    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	vec4 result = imageLoad(u_OutputColor, texCoords).rgba;
	result.rgb  = result.rgb * weight[0];

	for(int j = 1; j < 5; j++)
	{
		result.rgb += imageLoad(u_OutputColor, texCoords + ivec2(0, j)).rgb * weight[j];
		result.rgb += imageLoad(u_OutputColor, texCoords - ivec2(0, j)).rgb * weight[j];
	}
	
	imageStore(u_OutputColor, texCoords, result);
};


subroutine (Stage) 
void Combine()
{
	  vec4 first   = imageLoad(u_OutputColor,   ivec2(gl_GlobalInvocationID.xy));
	  vec4 second  = imageLoad(u_OutputColorI,  ivec2(gl_GlobalInvocationID.xy));
	  imageStore(u_OutputColorI, ivec2(gl_GlobalInvocationID.xy), first + second);
};

subroutine (Stage) 
void UpsempleTent()
{
	ivec4 d =  ivec4(1, 1, 1, 0) ;
	ivec2 mipCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 sourceCoords = mipCoords / 2;
	
    vec4 s;
	s  = imageLoad(u_InputColor, sourceCoords - d.xy);
    s += imageLoad(u_InputColor, sourceCoords - d.wy) * 2.0;
    s += imageLoad(u_InputColor, sourceCoords - d.zy);

    s += imageLoad(u_InputColor, sourceCoords + d.zw) * 2.0;
    s += imageLoad(u_InputColor, sourceCoords 	    ) * 4.0;
    s += imageLoad(u_InputColor, sourceCoords + d.xw) * 2.0;

    s += imageLoad(u_InputColor, sourceCoords + d.zy);
    s += imageLoad(u_InputColor, sourceCoords + d.wy) * 2.0;
    s += imageLoad(u_InputColor, sourceCoords + d.xy);

	imageStore(u_OutputColor, mipCoords, imageLoad(u_OutputColor, mipCoords) + (s  * (1.0 / 16.0)));
};

subroutine (Stage) 
void  DownsampleBox()
{
	ivec4 d =  ivec4(-1, -1, 1, 1);
	ivec2 mipCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 sourceCoords = mipCoords * 2;
	
	vec4 s;
	s  = imageLoad(u_InputColor, sourceCoords + d.xy);
    s += imageLoad(u_InputColor, sourceCoords + d.zy);
    s += imageLoad(u_InputColor, sourceCoords + d.xw);
    s += imageLoad(u_InputColor, sourceCoords + d.zw);

	imageStore(u_OutputColor, mipCoords, s * (1.0 / 4.0));
};
subroutine (Stage) 
void  DownsampleBox13()
{
	ivec2 mipCoords 	= ivec2(gl_GlobalInvocationID.xy);
    ivec2 sourceCoords 	= mipCoords * 2;

	vec4 A = imageLoad(u_InputColor, sourceCoords ivec2(-2,  0));
    vec4 B = imageLoad(u_InputColor, sourceCoords ivec2(-2,  0));
    vec4 C = imageLoad(u_InputColor, sourceCoords ivec2(-2,  0));
    vec4 D = imageLoad(u_InputColor, sourceCoords ivec2(-2,  0));
    vec4 E = imageLoad(u_InputColor, sourceCoords ivec2(-2,  0));
    vec4 F = imageLoad(u_InputColor, sourceCoords * ivec2(-2,  0));
    vec4 G = imageLoad(u_InputColor, sourceCoords				 );                             
    vec4 H = imageLoad(u_InputColor, sourceCoords * ivec2( 0,  0));
    vec4 I = imageLoad(u_InputColor, sourceCoords);
    vec4 J = imageLoad(u_InputColor, sourceCoords);
    vec4 K = imageLoad(u_InputColor, sourceCoords);
    vec4 L = imageLoad(u_InputColor, sourceCoords);
    vec4 M = imageLoad(u_InputColor, sourceCoords);

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4    o  = (D + E + I + J) * div.x;
    		o += (A + B + G + F) * div.y;
    		o += (B + C + H + G) * div.y;
    		o += (F + G + L + K) * div.y;
    		o += (G + H + M + L) * div.y;

 	imageStore(u_OutputColor, mipCoords, o);
};

subroutine uniform Stage u_Stage;
void main()
{
	u_Stage();
}