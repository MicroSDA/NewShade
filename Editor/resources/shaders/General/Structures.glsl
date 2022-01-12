
struct Camera
{
	mat4 ViewProjection;
	mat4 View;
	mat4 Projection;
	vec3 Position;
	vec3 Forward;
};

struct DirectLight
{
	vec3 Direction;
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	vec3 Position;
	mat4 ViewMatrix;
};

struct PointLight
{
	vec3		Position;
	vec3    AmbientColor;
	vec3    DiffuseColor;
	vec3    SpecularColor;
	float		Constant;
	float		Linear;
	float		Qaudratic;
};

struct SpotLight
{
	vec3 		Position;
	vec3 		Direction;
	vec3    AmbientColor;
	vec3    DiffuseColor;
	vec3    SpecularColor;
	float		Constant;
	float		Linear;
	float		Qaudratic;
  float   MinAngle;
  float   MaxAngle;
};

struct Material
{
	vec3  AmbientColor;
	vec3  DiffuseColor;
	vec3  SpecularColor;
	vec3  TransparentColor;
	float Emissive;
	float Shinines;
	float ShininesStrength;
	float Opacity;
	float Refracti;
};

struct DirectLightCascade
{
	mat4  ViewMatrix;
	float SplitDistance;
};
struct SpotLightCascade
{
	mat4  ViewMatrix;
};