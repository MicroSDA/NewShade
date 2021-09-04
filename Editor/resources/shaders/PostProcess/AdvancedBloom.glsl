#type compute
#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;

layout (rgba8,     binding = 0) uniform coherent  image2D u_InputColor;
layout (rgba8,     binding = 1) uniform coherent  image2D u_OutputColor;

//layout (binding = 2) uniform sampler2D u_InputColorS;
//layout (binding = 3) uniform sampler2D u_OutputColorS;
//layout (rgba8,     binding = 2) uniform coherent  image2D u_OutputColorI;


void BlurV()
{
	ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);

    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	vec4 result = imageLoad(u_InputColor, texCoords).rgba;
	result.rgb  = result.rgb * weight[0];

	for(int j = 1; j < 5; j++)
	{
		result.rgb += imageLoad(u_InputColor, texCoords + ivec2(0, j)).rgb * weight[j];
		result.rgb += imageLoad(u_InputColor, texCoords - ivec2(0, j)).rgb * weight[j];
	}
	
	imageStore(u_OutputColor, texCoords, result);
};
void BlurH()
{
	ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);

    float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	vec4 result = imageLoad(u_InputColor, texCoords).rgba;
	result.rgb  = result.rgb * weight[0];

	for(int j = 1; j < 5; j++)
	{
		result.rgb += imageLoad(u_InputColor, texCoords + ivec2(j, 0)).rgb * weight[j];
		result.rgb += imageLoad(u_InputColor, texCoords - ivec2(j, 0)).rgb * weight[j];
	}
	
	imageStore(u_OutputColor, texCoords, result);
};

vec4 DownsemplingBox4(ivec2 inputCoord)
{
	ivec4 offset       = ivec4(-1, -1,  1,  1);

    vec4 s;
    s =  imageLoad(u_InputColor, inputCoord + offset.xy);
    s += imageLoad(u_InputColor, inputCoord + offset.zy);
    s += imageLoad(u_InputColor, inputCoord + offset.xw);
    s += imageLoad(u_InputColor, inputCoord + offset.zw);

    return  s * (1.0 / 4.0);
 	//imageStore(u_OutputColor, outputCoords,  s * (1.0 / 4.0));
};

//http://goo.gl/eomGso
// . . . . . . .
// . A . B . C .
// . . D . E . .
// . F . G . H .
// . . I . J . .
// . K . L . M .
// . . . . . . .
vec4 DownsampleBox13(ivec2 inputCoord)
{

	vec4 A = imageLoad(u_InputColor, inputCoord + ivec2(-2, -2));
    vec4 B = imageLoad(u_InputColor, inputCoord + ivec2( 0, -2));
    vec4 C = imageLoad(u_InputColor, inputCoord + ivec2( 2, -2));
    vec4 D = imageLoad(u_InputColor, inputCoord + ivec2(-1, -1));
    vec4 E = imageLoad(u_InputColor, inputCoord + ivec2( 1, -1));
    vec4 F = imageLoad(u_InputColor, inputCoord + ivec2(-2,  0));

    vec4 G = imageLoad(u_InputColor, inputCoord				   );

    vec4 H = imageLoad(u_InputColor, inputCoord + ivec2( 2,  0));
    vec4 I = imageLoad(u_InputColor, inputCoord + ivec2(-1, -1));
    vec4 J = imageLoad(u_InputColor, inputCoord + ivec2( 1,  1));
    vec4 K = imageLoad(u_InputColor, inputCoord + ivec2(-2,  2));
    vec4 L = imageLoad(u_InputColor, inputCoord + ivec2( 0,  2));
    vec4 M = imageLoad(u_InputColor, inputCoord + ivec2( 2,  2));

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 o  = (D + E + I + J) * div.x;
    	 o += (A + B + G + F) * div.y;
    	 o += (B + C + H + G) * div.y;
    	 o += (F + G + L + K) * div.y;
    	 o += (G + H + M + L) * div.y;

    return o;
   // imageStore(u_OutputColor, outputCoords,  o);
};

vec4 UpsampleBox(ivec2 inputCoord)
{

	ivec4 offset       = ivec4(-1, -1,  1,  1) * 1;// 2 is scale

    vec4 s;
    s =  imageLoad(u_InputColor, inputCoord + offset.xy);
    s += imageLoad(u_InputColor, inputCoord + offset.zy);
    s += imageLoad(u_InputColor, inputCoord + offset.xw);
    s += imageLoad(u_InputColor, inputCoord + offset.zw);

    return s * (1.0 / 4.0);
 	//imageStore(u_OutputColor, outputCoords,  s * (1.0 / 4.0));
};

vec4 UpsampleTent(ivec2 inputCoord)
{
	ivec4 offset       = ivec4(1, 1,  -1,  1) * 1; // 2 is scale

	vec4 s;
	 s =  imageLoad(u_InputColor, inputCoord - offset.xy);
	 s += imageLoad(u_InputColor, inputCoord - offset.wy) * 2.0;
	 s += imageLoad(u_InputColor, inputCoord - offset.zy);

	 s += imageLoad(u_InputColor, inputCoord + offset.zw) * 2.0;
	 s += imageLoad(u_InputColor, inputCoord 			) * 4.0;
	 s += imageLoad(u_InputColor, inputCoord + offset.xw) * 2.0;

	 s += imageLoad(u_InputColor, inputCoord + offset.zy);
	 s += imageLoad(u_InputColor, inputCoord + offset.wy) * 2.0;
	 s += imageLoad(u_InputColor, inputCoord + offset.xy);

	 return s * (1.0 / 16.0);
	 //imageStore(u_OutputColor, outputCoords,  s * (1.0 / 16.0));
};

vec4 QuadraticThreshold(vec4 color, float threshold, vec3 curve)
{
    // Pixel brightness
    float br =  max(max(color.r, color.g), color.b);

    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, 1.0e-4);

    return color;
};

vec4 Hdr(vec4 color)
{

	//ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	//vec4  color 	   = imageLoad(u_InputColor, inputCoord);
	color 	   		   = min(color, 65504.0);

	color = min(color, 1.0); // clamp to max
    //color = QuadraticThreshold(color, _Threshold.x, _Threshold.yzw);
    color = QuadraticThreshold(color, 0.0, vec3(0.2126, 0.7152, 0.0722));

    return color;
    //imageStore(u_OutputColor, inputCoord,  color);
};

subroutine void Stage();
subroutine (Stage) 
void PreF13()
{
    ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords = inputCoord / 2;

	imageStore(u_OutputColor, outputCoords,  Hdr(DownsampleBox13(inputCoord)));
};
subroutine (Stage) 
void PreF4()
{
 	ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords = inputCoord / 2;

	imageStore(u_OutputColor, outputCoords,  Hdr(DownsemplingBox4(inputCoord)));
};
subroutine (Stage) 
void DownSample13()
{
 	ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords = inputCoord / 2;

	imageStore(u_OutputColor, outputCoords,  DownsampleBox13(inputCoord));
};
subroutine (Stage) 
void DownBox()
{
 	ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords = inputCoord / 2;

	imageStore(u_OutputColor, outputCoords,  DownsemplingBox4(inputCoord));
};
subroutine (Stage) 
void UpTent()
{
	ivec2 inputCoord   	= ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords 	= inputCoord * 2;
	vec4 first   		= UpsampleTent(inputCoord);
	vec4 second  		= imageLoad(u_OutputColor, outputCoords);

	imageStore(u_OutputColor, outputCoords, first + second);
};
subroutine (Stage) 
void UpBox()
{
	ivec2 inputCoord   	= ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords 	= inputCoord * 2;
	vec4 first   		= UpsampleBox(inputCoord);
	vec4 second  		= imageLoad(u_OutputColor, outputCoords);
	
	imageStore(u_OutputColor, outputCoords, first + second);
};
subroutine (Stage) 
void Combine()
{
	// Fix this, need try to separte combine from upscae
	ivec2 inputCoord   = ivec2(gl_GlobalInvocationID.xy);
	ivec2 outputCoords = inputCoord * 2;
	vec4 first   	   = UpsampleTent(inputCoord);
	vec4 second  	   = imageLoad(u_OutputColor, outputCoords);

	imageStore(u_OutputColor, outputCoords, first + second);
};
subroutine (Stage) 
void BV()
{
	BlurV();
};
subroutine (Stage) 
void BH()
{
	BlurH();
};
subroutine uniform Stage s_Stage;

void main()
{
	//ivec2 texCoords    = ivec2(gl_GlobalInvocationID.xy);
	//ivec2 sourceCoords = texCoords / 2;
	//vec4  inputImage   = imageLoad(u_InputColor, texCoords);
	s_Stage();

	//vec4 data = textureLod(u_InputColorS, ivec2(gl_GlobalInvocationID.xy).xy, 2);
	//imageStore(u_OutputColor, ivec2(gl_GlobalInvocationID.xy).xy, data + vec4(0, 0.5, 0, 0));
	//DownsampleBox13();
	//DowsemplingBox4();
	//UpsampleBox();
	//UpsampleTent();
	//Hdr();
	//imageStore(u_OutputColor, sourceCoords, inputImage + vec4(0, 0.5, 0, 0));
}