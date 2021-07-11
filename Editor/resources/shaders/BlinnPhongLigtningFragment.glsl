#include "resources/shaders/Shade.glsl"


#define MAX_POINT_LIGHTNING_SOURCES 10
#define MAX_SPOT_LIGHTNING_SOURCES 	10

struct Material
{
	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	float Shinines;
	float ShininesStrength;
};

struct DirectLight
{
	vec3 Direction;
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
};
struct PointLight
{
	DirectLight Light;
	vec3		Position;
	float		Constant;
	float		Linear;
	float		Qaudratic;
	int		 	Id; // idk for what
};

struct SpotLight
{
  PointLight Light;
  float      MinAngle;
  float      MaxAngle;
  int		 Id;
};
struct Lighting
{
	DirectLight _DirectLight;
	PointLight  _PointLights[MAX_POINT_LIGHTNING_SOURCES];
	SpotLight 	_SpotLights[MAX_SPOT_LIGHTNING_SOURCES];
	int         _PointLightsCount;
	int         _SpotLightsCount;
};
vec3 CalculateTBNNormal(vec3 normalMap, mat3 TBNMatrix)
{
   vec3 TBNNormal = normalMap;
   TBNNormal = 2.0 * TBNNormal  - vec3(1.0, 1.0, 1.0);   
   TBNNormal = normalize(TBNMatrix * TBNNormal);
   return TBNNormal;
}

vec4 ProcessDirectLight(vec3 normal, DirectLight light, Material material, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture)
{
	vec4 ambientColor	 = vec4((diffuseTexture.rgb * material.AmbientColor * light.AmbientColor), diffuseTexture.a);
	vec4 diffuseColor	 = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 specularColor	 = vec4(0.0, 0.0, 0.0, 0.0);

	float diffuseShading = dot(normal, -light.Direction);

	if(diffuseShading > 0.0)
	{
		diffuseColor			= vec4((diffuseTexture.rgb * material.DiffuseColor * light.DiffuseColor * diffuseShading), diffuseTexture.a);
		vec3 lightReflection	= reflect(light.Direction, normal);
		float specularShading	= dot(toCameraDirection, lightReflection);

		if(specularShading > 0.0)
		{
			specularShading		= pow(specularShading, material.Shinines);
			specularColor		= vec4((specularTexture.rgb * material.SpecularColor * light.SpecularColor * material.ShininesStrength * specularShading), specularTexture.a);
		}
	}

	return vec4(ambientColor + diffuseColor + specularColor);
}

vec4 ProcessPointLight(vec3 normal, PointLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture)
{
	vec3 lightDirection		= vertex - light.Position;
	float distanceBetween	= length(lightDirection);
	lightDirection			= normalize(lightDirection);
	PointLight pointLight	= light;
	pointLight.Light.Direction	= lightDirection;

	vec4 totalColor			= ProcessDirectLight(normal, pointLight.Light, material, toCameraDirection, diffuseTexture, specularTexture);
	float attenuation       = pointLight.Constant + pointLight.Linear * distanceBetween + pointLight.Qaudratic * (distanceBetween * distanceBetween);

	return totalColor / attenuation;
}

vec4 ProcessSpotLight(vec3 normal, SpotLight light, Material material, vec3 vertex, vec3 toCameraDirection, vec4 diffuseTexture, vec4 specularTexture)
{
	const float Smooth			= 2; // Harcoded 
    vec3  lightDirection		= normalize(vertex - light.Light.Position);
    float spotFactor			= dot(lightDirection, light.Light.Light.Direction);
    
    if (spotFactor > light.MaxAngle)
    { 
        float epsilon			= light.MinAngle - light.MaxAngle;
        spotFactor				= smoothstep(0.0, Smooth, (spotFactor - light.MaxAngle) / epsilon);
        SpotLight spotLight		= light;
        vec4 totalColor			= ProcessPointLight(normal, light.Light, material, vertex, toCameraDirection, diffuseTexture, specularTexture);

        return totalColor * spotFactor;
    }
    else
    {
        return vec4(0, 0, 0, 0);
    }
}

uniform Lighting          u_Lighting;
uniform Material          u_Material;