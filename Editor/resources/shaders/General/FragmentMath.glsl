
// For BillinPhong lighting calculation
vec3 Get_TBNNormal(vec3 Normal, mat3 TBN_Matrix)
{
   vec3 TBN_Normal = Normal;
   TBN_Normal = 2.0 * TBN_Normal  - vec3(1.0, 1.0, 1.0);   
   TBN_Normal = normalize(TBN_Matrix * TBN_Normal);
   return TBN_Normal;
}
// Need to refactor this
struct Cascade
{
    mat4  ViewMatrix;
    float Distance;
};

float ComputeShadowCascade(Cascade cascade[4], sampler2DArray shadowMap, mat4 CameraView, vec3 fragPosWorldSpace, vec3 Normal, vec3 lightDirection)
{
    const int cascadeCount = 4;

    // select cascade layer
    vec4 fragPosViewSpace = CameraView * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer =-1;
    for (int i = 0; i < cascadeCount; ++i)
    {
       if (depthValue < cascade[i].Distance)
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }
    vec4 fragPosLightSpace = cascade[layer].ViewMatrix * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (currentDepth  > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

    const float farPlane = 1000.0;// Hardcoded need to get from camera !!!!

    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * 0.5);
    }
    else
    {
        bias *= 1 / (cascade[layer].Distance * 0.5);
    }
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
              float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r; 
              //float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize, 0).r; 
              shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }
        
    return shadow;
}
float ShadowCalculationTest(sampler2DArray shadowMap, mat4 CameraView, vec3 fragPosWorldSpace, vec3 Normal, vec3 lightDirection, mat4 lightViewMatrix)
{
    // select cascade layer
    vec4 fragPosViewSpace = CameraView * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    vec4 fragPosLightSpace = lightViewMatrix * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    if (currentDepth  > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    float bias = max(0.0005 * (1.0 - dot(normal, lightDirection)), 0.00005);
  
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
              float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, 0)).r; 
              //float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize, 0).r; 
              shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }
        
    return shadow;
}

float ShadowCalculation(sampler2D shadowMap, vec3 lightPos, vec3 Normal, vec3 FragPos, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
   // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}