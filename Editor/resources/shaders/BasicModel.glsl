#type vertex
#version 460 core
#include "resources/shaders/BlinnPhongLigtningVertex.glsl"

layout (location = 0) in vec3  Position;
layout (location = 1) in vec2  UV_Coordinates;
layout (location = 2) in vec3  Normal;
layout (location = 3) in vec3  Tangent;
layout (location = 4) in mat4  Transform;

layout (location = 0) out vec2 out_UV_Coordinates;
layout (location = 1) out vec3 out_Normal;
layout (location = 2) out vec3 out_CameraPosition;
layout (location = 3) out vec3 out_Vertex;
	
//uniform mat4 Transform;
struct Camera
{
	mat4 _View;
	mat4 _Projection;
	vec3 _Position;
	vec3 _Forward;
};

layout(std140, binding = 0) uniform UniformCamera
{
	Camera u_Camera;
};

layout(std140, binding = 1) uniform UniformClipDistance
{
	vec4 u_ClipDistance;
};

out mat3 out_TBN;

 
void main()
{
	
	gl_Position = u_Camera._Projection * u_Camera._View * Transform *  vec4(Position, 1.0f);
	gl_ClipDistance[0] = dot(u_Camera._View * vec4(Position, 1.0f), u_ClipDistance);
	
	
	out_UV_Coordinates = UV_Coordinates;
	out_Normal  = (Transform * vec4(Normal, 0.0)).xyz;
	out_TBN =  CalculateTBNMatrix(Transform, Normal, Tangent);
	out_Vertex = (Transform * vec4(Position, 1.0f)).xyz;
	out_CameraPosition  = u_Camera._Position;
	//InstanceID = gl_InstanceID;
}

#type fragment
#version 460 core
#include "resources/shaders/BlinnPhongLigtningFragment.glsl"


struct Sampler2D
{
	int       Type;
	sampler2D Sampler;
};

struct Textures
{
	Sampler2D Samplers[3];
};



layout (location = 0)  in vec2 UV_Coordinates;
layout (location = 1)  in vec3 Normal;
layout (location = 2)  in vec3 CameraPosition;
layout (location = 3)  in vec3 Vertex;


//layout (binding  = 0)  uniform sampler2D Textures[3];
layout (location = 0)  out vec4 ColorAttachment;

uniform Textures textures;

in mat3 out_TBN;



void main()
{

		
	    vec3 TBNNormal = CalculateTBNNormal(texture(textures.Samplers[TEXTURE_NORMAL].Sampler, UV_Coordinates).rgb, out_TBN);
		vec3 ToCameraDirection = normalize(CameraPosition - Vertex);

		vec4 TotalColor = ProcessDirectLight(TBNNormal, u_Lighting._DirectLight, u_Material, ToCameraDirection, texture(textures.Samplers[TEXTURE_DIFFUSE].Sampler, UV_Coordinates).rgba, texture(textures.Samplers[TEXTURE_SPECULAR].Sampler, UV_Coordinates).rgba);
		//vec4 TotalColor = ProcessDirectLight(Normal, u_Lighting._DirectLight, u_Material, ToCameraDirection, vec4(1,1,1,1), vec4(1,1,1,1));
		for(int i = 0; i < u_Lighting._PointLightsCount; i++)
		{
			TotalColor += ProcessPointLight(TBNNormal, u_Lighting._PointLights[i], u_Material, Vertex, ToCameraDirection, texture(textures.Samplers[TEXTURE_DIFFUSE].Sampler, UV_Coordinates).rgba, texture(textures.Samplers[TEXTURE_SPECULAR].Sampler, UV_Coordinates).rgba);
			if(i >= MAX_POINT_LIGHTNING_SOURCES - 1)
				break;
		}
		for(int i = 0; i < u_Lighting._SpotLightsCount; i++)
		{
			TotalColor+= ProcessSpotLight(TBNNormal, u_Lighting._SpotLights[i], u_Material, Vertex, ToCameraDirection, texture(textures.Samplers[TEXTURE_DIFFUSE].Sampler, UV_Coordinates).rgba, texture(textures.Samplers[TEXTURE_SPECULAR].Sampler, UV_Coordinates).rgba);
			if(i >= MAX_SPOT_LIGHTNING_SOURCES - 1)
				break;
		}

		ColorAttachment = vec4(pow(TotalColor.rgb, vec3(0.40/1)), 1.0 ); // m_TotalColor.a for imgui alpha shoudl be as 1.0 
		//ColorAttachment = vec4(TotalColor.rgb, 1.0); // m_TotalColor.a for imgui alpha shoudl be as 1.0 
		//ColorAttachment = texture(Textures[TEXTURE_DIFFUSE], UV_Coordinates).rgba;
}