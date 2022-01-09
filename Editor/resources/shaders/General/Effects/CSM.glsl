
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
    if (CurentDepth > 1.0) // Tweek ?
        return 0.0;
    /* Calculate bias */
    float Offset = 0.05;
    if(CascadeLayer == CascadeCount - 1)
        Offset /= SplitDistance * 2.0;
    else
        Offset /= SplitDistance * 1.9;
    /*if(CascadeLayer != 0)
        Offset /= SplitDistance;*/

    float Bias = max(Offset * (1.0 - normalize(dot(Normal, LightDirection))), Offset);
    //Bias       = max(Bias, Offset);
    //return Bias / 0.001;
    return PCF_DirectLight(ShadowMap, ProjectionCoords, CurentDepth, Bias, CascadeLayer);
}

float SM_SpotLight(sampler2D ShadowMap, mat4 LightViewMatrix, mat4 CameraView, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightDirection)
{
     vec4 FragPosLightSpace = LightViewMatrix * vec4(FragPosWorldSapce, 1.0);
    /* Perspective divide */
    vec3 ProjectionCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    /* Set between 0 - 1 range */
    ProjectionCoords  = ProjectionCoords * 0.5 + 0.5;
    float CurentDepth = ProjectionCoords.z;
    if (CurentDepth  > 1.0) // Tweek ?
        return 0.0;
    /* Calculate bias */
    float Bias = max(0.025 * (1.0 - dot(Normal, LightDirection)), 0.0005);
    return PCF_SpotLight(ShadowMap, ProjectionCoords, CurentDepth, Bias);
}