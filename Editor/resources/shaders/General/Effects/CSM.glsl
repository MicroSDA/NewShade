
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
    const int   SamplesCount   = 3;
    float       Value        = 0.0;
    vec2        TexelSize    = 1.0 / vec2(textureSize(ShadowMap, 0));

    for(int x = -SamplesCount; x < SamplesCount; x++)
    {  
        for(int y = -SamplesCount; y < SamplesCount; y++)
        {
            float _Depth = texture(ShadowMap, vec3(ProjectionCoords.xy + vec2(x, y) * TexelSize, CascadeLayer)).r;
            vec2  Moments = vec2(_Depth, _Depth * _Depth);
            float P = step(Depth, Moments.x);
            float Variance = max(Moments.y - Moments.x * Moments.x, 0.00002);
            float D = Depth - Moments.x;
            float Max = LineStep(0.99, 1.0, Variance / (Variance + D * D));
            Value += max(P, Max);
        }
    }

    Value /= float(SamplesCount * max(CascadeLayer, 1));
    return 1.0 - min(Value, 1.0);
}

float Variance_SpotLight(sampler2D ShadowMap, vec3 ProjectionCoords, float Depth)
{
    const int   SamplesCount   = 1;
    float       Value        = 0.0;
    vec2        TexelSize    = 1.0 / vec2(textureSize(ShadowMap, 0));

    for(int x = -SamplesCount; x < SamplesCount; x++)
    {  
        for(int y = -SamplesCount; y < SamplesCount; y++)
        {
            float _Depth = texture(ShadowMap, vec2(ProjectionCoords.xy + vec2(x, y) * TexelSize)).r; 
            vec2  Moments = vec2(_Depth, _Depth * _Depth);
            float P = step(Depth, Moments.x);
            float Variance = max(Moments.y - Moments.x * Moments.x, 0.000000002);
            float D = Depth - Moments.x;
            float Max = LineStep(0.9, 1.0, Variance / (Variance + D * D));
            Value += max(P, Max);
        }
    }

    Value /= SamplesCount * 2;
    return min(Value, 1.0);
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
        return 0.0;

    return Variance_DirectLight(ShadowMap, ProjectionCoords, CurentDepth, CascadeLayer);

    /* Calculate bias */
    /*float Offset = 0.05;
    if(CascadeLayer == CascadeCount - 1)
        Offset /= SplitDistance * 2.0;
    else
        Offset /= SplitDistance * 1.9;
    float Bias = max(Offset * (1.0 - normalize(dot(Normal, LightDirection))), Offset);
    return PCF_DirectLight(ShadowMap, ProjectionCoords, CurentDepth, Bias, CascadeLayer);*/
}

float SM_SpotLight(sampler2D ShadowMap, mat4 LightViewMatrix, mat4 CameraView, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightDirection)
{
     vec4 FragPosLightSpace = LightViewMatrix * vec4(FragPosWorldSapce, 1.0);
    /* Perspective divide */
    vec3 ProjectionCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    /* Set between 0 - 1 range */
    ProjectionCoords  = ProjectionCoords * 0.5 + 0.5;
    float CurentDepth = ProjectionCoords.z;
   if (!ShadowInRange(ProjectionCoords.z) || !ShadowInRange(ProjectionCoords.x) || !ShadowInRange(ProjectionCoords.y))
   {
       return 1.0;
   }
        
    /* Calculate bias */
    float Bias = max(0.25 * (1.0 - dot(Normal, LightDirection)), 0.0005);
    return Variance_SpotLight(ShadowMap, ProjectionCoords, CurentDepth);
}