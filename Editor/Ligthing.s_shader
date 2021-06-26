struct BaseLight
{
   vec3  Direction;
   vec3  ColorAmbient;
   vec3  ColorDiffuse;
   vec3  ColorSpecular;
   float ShininesStrength; // move To material
};
struct PointLight 
{
   BaseLight Light;
   vec3  Position;
   float Constant;
   float Linear;                                                                   
   float Qaudratic;    
};
struct SpotLight
{
  PointLight Light;
  float      MinAngle;
  float      MaxAngle;
};
struct Material 
{
   vec3  colorAmbient;
   vec3  colorDiffuse;
   vec3  colorSpecular;
   vec3  transparentMask;
   float shinines;
   float shininesStrength;
 
};

vec4 ProcessGeneralLight(vec3 Normals, BaseLight Light, Material material, vec3 ToCameraDirection, vec4 DiffuseTexture, vec4 SpecularTexture)
{                                                          //Material
    vec4  m_AmbientColor   = vec4((DiffuseTexture.rgb * material.colorAmbient * Light.ColorAmbient), DiffuseTexture.a);
    vec4  m_DiffuesColor   = vec4(0.0, 0.0, 0.0, 0.0);
    vec4  m_SpecularColor  = vec4(0.0, 0.0, 0.0, 0.0);

    float m_DiffuesShading = dot(Normals, -Light.Direction);

    if(m_DiffuesShading > 0.0)
    {                                                       // Material
       m_DiffuesColor          = vec4((DiffuseTexture.rgb * material.colorDiffuse * Light.ColorDiffuse * m_DiffuesShading), DiffuseTexture.a);
       vec3 LightReflect       = normalize(reflect(Light.Direction, Normals)); // normalize(reflect(Light.Direction, Normals));
       float m_SpecularShading = dot(ToCameraDirection, LightReflect);

       if(m_SpecularShading > 0.0)
       {
          m_SpecularShading = pow(m_SpecularShading, material.shinines);
                                                         //Material
          m_SpecularColor = vec4((SpecularTexture.rgb * material.colorSpecular * Light.ColorSpecular * material.shininesStrength * m_SpecularShading), SpecularTexture.a);
      }

    }

    return vec4(m_AmbientColor + m_DiffuesColor + m_SpecularColor);
}

vec4 ProcessPointLight(vec3 Normals, PointLight pointLight, Material material, vec3 ModelPosition, vec3 ToCameraDirection, vec4 DiffuseTexture, vec4 SpecularTexture)
{
    vec3  m_LightDirection  = ModelPosition - pointLight.Position;
    float m_Distance        = length(m_LightDirection);
    m_LightDirection        = normalize(m_LightDirection);
    PointLight m_Light      = pointLight;
    m_Light.Light.Direction = m_LightDirection;

    vec4  m_Color = ProcessGeneralLight(Normals, m_Light.Light, material, ToCameraDirection, DiffuseTexture, SpecularTexture);
    float m_Attenuation =  pointLight.Constant + pointLight.Linear * m_Distance + pointLight.Qaudratic * (m_Distance * m_Distance);

    return m_Color / m_Attenuation;
}

vec4 ProcessSpotLight(vec3 Normals, SpotLight spotLight, Material material, vec3 ModelPosition, vec3 ToCameraDirection, vec4 DiffuseTexture, vec4 SpecularTexture)
{
    const float m_Smooth = 2; // Harcoded 
    vec3  m_LightDirection = normalize(ModelPosition - spotLight.Light.Position);
    float m_SpotFactor     = dot(m_LightDirection,  spotLight.Light.Light.Direction);
    
    if (m_SpotFactor > spotLight.MaxAngle)
    { 
        float m_Epsilon     = spotLight.MinAngle - spotLight.MaxAngle;
        m_SpotFactor        = smoothstep(0.0, m_Smooth, (m_SpotFactor - spotLight.MaxAngle) / m_Epsilon);
        SpotLight m_Light   = spotLight;
        vec4 m_Color        = ProcessPointLight(Normals, spotLight.Light, material, ModelPosition, ToCameraDirection, DiffuseTexture, SpecularTexture);
        return m_Color * m_SpotFactor;
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}