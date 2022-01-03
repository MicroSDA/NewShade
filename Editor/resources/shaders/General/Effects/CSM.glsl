
float PCF(sampler2DArray ShadowMap, vec3 ProjectionCoords, float Depth, float Bias, int CascadeLayer)
{
    float       Value               = 0.0;
    vec2        TexelSize           = 1.0 / vec2(textureSize(ShadowMap, 0));
    const int   SamplesCount        = 3;

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
/* Cascaded Shadow Mapping */
float CSM_DirectLight(sampler2DArray ShadowMap, mat4 LightViewMatrix, int CascadeLayer, float SplitDistance, mat4 CameraView, vec3 FragPosWorldSapce, vec3 Normal, vec3 LightDirection)
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
    float Bias = max(0.05 * (1.0 - dot(Normal, LightDirection)), 0.005);
    if (CascadeLayer == 4)
        Bias *= 1.0 / (1000 * 0.5);
    else
        Bias *= 1.0 / (SplitDistance * 0.5); // Bias *= 1.0 / (SplitDistance * 0.5);
        
    return PCF(ShadowMap, ProjectionCoords, CurentDepth, Bias, CascadeLayer);
}