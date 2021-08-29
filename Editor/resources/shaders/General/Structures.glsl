
struct Camera
{
	mat4 _View;
	mat4 _Projection;
	vec3 _Position;
	vec3 _Forward;
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
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Shinines;
	float ShininesStrength;
};