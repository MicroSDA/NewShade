
vec2 PoissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
);

bool ShadowInRange(float value)
{
    return value >= 0.0 && value <= 1.0;
}

float LineStep(float low, float hight, float v)
{
    return clamp((v-low)/(hight-low), 0.0, 1.0);
}

float Variance_DirectLight(sampler2DArray ShadowMap, vec3 ProjectionCoords, float Depth, int CascadeLayer)
{ 
    const int   SamplesCount   = 16;
    float       Value          = 0.0;
    vec2        TexelSize      = 1.0 / vec2(textureSize(ShadowMap, 0));

    for(int i = 0; i < SamplesCount; i++)
    {
         float _Depth = texture(ShadowMap, vec3(ProjectionCoords.xy + (PoissonDisk[i] * TexelSize), CascadeLayer)).r;
         vec2  Moments = vec2(_Depth, _Depth * _Depth);
         float P = step(Depth, Moments.x);
         float Variance = max(Moments.y - Moments.x * Moments.x, 0.00002);
         float D = Depth - Moments.x;
         float Max = LineStep(0.99, 1.0, Variance / (Variance + D * D));
         Value += max(P, Max);
    }
    Value /= SamplesCount;
    return min(Value, 1.0);
}

float Variance_SpotLight(sampler2DArray ShadowMap, vec3 ProjectionCoords, float Depth, int CascadeLayer)
{
    const int   SamplesCount = 16;
    float       Value        = 0.0;
    vec2        TexelSize    = 1.0 / vec2(textureSize(ShadowMap, 0));
    for(int i = 0; i < SamplesCount; i++)
    {
         float _Depth = texture(ShadowMap, vec3(ProjectionCoords.xy + (PoissonDisk[i] * TexelSize), CascadeLayer)).r;
         vec2  Moments = vec2(_Depth, _Depth * _Depth);
         float P = step(Depth, Moments.x);
         float Variance = max(Moments.y - Moments.x * Moments.x, 0.00002);
         float D = Depth - Moments.x;
         float Max = LineStep(0.999, 1.0, Variance / (Variance + D * D));
         Value += max(P, Max);
    }
    Value /= SamplesCount;
    return min(Value, 1.0);
}

float Variance_PointLight(samplerCubeArray ShadowMap, vec3 ProjectionCoords, float Depth, float Distance, int CascadeLayer)
{
    const int   SamplesCount = 16;
    float       Value        = 0.0;
    vec2        TexelSize    = 1.0 / vec2(textureSize(ShadowMap, 0));
    int         Poisson      = 0;
    for(int i = 0; i < SamplesCount; i++)
    {
        float  _Depth = texture(ShadowMap,    vec4(vec3(ProjectionCoords.xy + vec2(PoissonDisk[i] * TexelSize * Depth), ProjectionCoords.z), CascadeLayer)).r * Distance;
        Value += Depth - clamp(_Depth, 0.0, 1.0) >= _Depth ? 1.0 : 0.0;  
    }
    Value /= SamplesCount;
    return 1.0 - Value;
}

float PCF_DirectLight(sampler2DArray ShadowMap, vec3 ProjectionCoords, float Depth, float Bias, int CascadeLayer)
{
    float       Value               = 0.0;
    vec2        TexelSize           = 1.0 / vec2(textureSize(ShadowMap, 0));
    const int   SamplesCount        = 1;

    for(int x = -SamplesCount; x < SamplesCount; x++)
    {  
        for(int y = -SamplesCount; y < SamplesCount; y++)
        {
            float _Depth = texture(ShadowMap, vec3(ProjectionCoords.xy + vec2(x, y) * TexelSize, CascadeLayer)).r; 
            Value += (Depth - Bias) > _Depth ? 1.0 : 0.0; 
        } 
    }

    return Value / (SamplesCount * 2);
}

float PCF_SpotLight(sampler2D ShadowMap, vec3 ProjectionCoords, float Depth, float Bias)
{
    float       Value               = 0.0;
    vec2        TexelSize           = 1.0 / vec2(textureSize(ShadowMap, 0));
    const int   SamplesCount        = 3;

    for(int x = -SamplesCount; x < SamplesCount; x++)
    {  
        for(int y = -SamplesCount; y < SamplesCount; y++)
        {
            float _Depth = texture(ShadowMap, vec2(ProjectionCoords.xy + vec2(x, y) * TexelSize)).r; 
            Value += (Depth - Bias) > _Depth ? 1.0 : 0.0; 
        } 
    }

    return Value / (SamplesCount * 2);
}
/* Cascaded Shadow Mapping */
float CSM_DirectLight(sampler2DArray ShadowMap, mat4 LightViewMatrix, int CascadeLayer, int CascadeCount, float SplitDistance, mat4 CameraView, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightDirection)
{
    vec4 FragPosLightSpace = LightViewMatrix * vec4(FragPosWorldSapce, 1.0);
    /* Perspective divide */
    vec3 ProjectionCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    /* Set between 0 - 1 range */
    ProjectionCoords  = ProjectionCoords * 0.5 + 0.5;
    float CurentDepth = ProjectionCoords.z;
    if (!ShadowInRange(ProjectionCoords.z) || !ShadowInRange(ProjectionCoords.x) || !ShadowInRange(ProjectionCoords.y)) // Tweek ?
        return 1.0;

    return Variance_DirectLight(ShadowMap, ProjectionCoords, CurentDepth, CascadeLayer);
}

float SM_PointLight(samplerCubeArray ShadowMap, int CascadeLayer, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightPosition, float Distance)
{
    vec3 FragPosLightSpace = FragPosWorldSapce - LightPosition;
    vec3 ProjectionCoords  = FragPosLightSpace.xyz;
    float CurentDepth      = length(FragPosLightSpace);
    if(CurentDepth <= 0.0 || CurentDepth > Distance)
        return 0.0;

    return Variance_PointLight(ShadowMap, ProjectionCoords, CurentDepth, Distance, CascadeLayer);
}

float SM_SpotLight(sampler2DArray ShadowMap, mat4 LightViewMatrix, int CascadeLayer, mat4 CameraView, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightDirection)
{
    vec4 FragPosLightSpace = LightViewMatrix * vec4(FragPosWorldSapce, 1.0);
    /* Perspective divide */
    vec3 ProjectionCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    /* Set between 0 - 1 range */
    ProjectionCoords  = ProjectionCoords * 0.5 + 0.5;
    float CurentDepth = ProjectionCoords.z;
    if (!ShadowInRange(ProjectionCoords.z) || !ShadowInRange(ProjectionCoords.x) || !ShadowInRange(ProjectionCoords.y))
        return 1.0;  
    /* Calculate bias */
    //float Bias = max(0.25 * (1.0 - dot(Normal, LightDirection)), 0.0005);
    return Variance_SpotLight(ShadowMap, ProjectionCoords, CurentDepth, CascadeLayer);
}