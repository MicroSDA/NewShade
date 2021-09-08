#type compute
#version 460 core
layout(local_size_x = 32 local_size_y = 32) in;

layout (rgba8,     binding = 0) uniform coherent  image2D u_InputColor;
layout (rgba8,     binding = 1) uniform coherent  image2D u_OutputColor;

layout (binding        = 2) uniform sampler2D u_sInputColor;
layout (rgba8, binding = 3) uniform image2D   u_sOutputColor;

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
vec4 DownBox13(vec2 uv, vec2 size, float lod)
{
	vec2 textlSize = 1.0 / size.xy;

	vec4 A = textureLod(u_sInputColor, uv + textlSize * vec2(-1.0, -1.0), lod);
    vec4 B = textureLod(u_sInputColor, uv + textlSize * vec2( 0.0,  0.0), lod);
    vec4 C = textureLod(u_sInputColor, uv + textlSize * vec2( 1.0, -1.0), lod);
    vec4 D = textureLod(u_sInputColor, uv + textlSize * vec2(-0.5, -0.5), lod);
    vec4 E = textureLod(u_sInputColor, uv + textlSize * vec2( 0.5, -0.5), lod);
    vec4 F = textureLod(u_sInputColor, uv + textlSize * vec2(-1.0,  0.0), lod);

    vec4 G = textureLod(u_sInputColor, uv, lod				   	             );

    vec4 H = textureLod(u_sInputColor, uv + textlSize * vec2( 1.0,  0.0), lod);
    vec4 I = textureLod(u_sInputColor, uv + textlSize * vec2(-0.5,  0.5), lod);
    vec4 J = textureLod(u_sInputColor, uv + textlSize * vec2( 0.5,  0.5), lod);
    vec4 K = textureLod(u_sInputColor, uv + textlSize * vec2(-1.0,  1.0), lod);
    vec4 L = textureLod(u_sInputColor, uv + textlSize * vec2( 0.0,  1.0), lod);
    vec4 M = textureLod(u_sInputColor, uv + textlSize * vec2( 1.0,  1.0), lod);

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

	//imageStore(u_OutputColor, outputCoords,  Hdr(DownsampleBox13(inputCoord)));
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

	//imageStore(u_OutputColor, outputCoords,  DownsampleBox13(inputCoord));
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
// . . . . . . .
// . A . B . C .
// . . D . E . .
// . F . G . H .
// . . I . J . .
// . K . L . M .
// . . . . . . .
vec4 DownBox(vec2 uv, vec2 size, float lod)
{
	vec4 offset = (1.0 / size.xyxy) * vec4(-1.0, -1.0, 1.0, 1.0);

    vec4 color;
    color =  textureLod(u_sInputColor, uv + offset.xy, lod);
    color += textureLod(u_sInputColor, uv + offset.zy, lod);
    color += textureLod(u_sInputColor, uv + offset.xw, lod);
    color += textureLod(u_sInputColor, uv + offset.zw, lod);

    return  color * (1.0 / 4.0);
 	//imageStore(u_OutputColor, outputCoords,  s * (1.0 / 4.0));
};
subroutine (Stage) 
void Test()
{
	ivec2 samplerPos 	= ivec2(gl_GlobalInvocationID.xy) / int(pow(2.0, 5.0)); // 2 в степени 5
	vec2 uv 			= (samplerPos + 0.5) / vec2(textureSize(u_sInputColor, 5));
	
	//imageStore(u_sOutputColor,  ivec2(gl_GlobalInvocationID.xy), DownBox(uv, textureSize(u_sInputColor, 5).xy, 5.0));
	imageStore(u_sOutputColor,  ivec2(gl_GlobalInvocationID.xy),   DownBox13(uv, textureSize(u_sInputColor, 5).xy, 5.0));
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