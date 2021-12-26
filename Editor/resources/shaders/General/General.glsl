#type vertex
#version 460 core
#include "resources/shaders/General/Structures.glsl"
#include "resources/shaders/General/VertexMath.glsl"
// Input attributes
layout (location = 0) in vec3  a_Position;
layout (location = 1) in vec2  a_UV_Coordinates;
layout (location = 2) in vec3  a_Normal;
layout (location = 3) in vec3  a_Tangent;
layout (location = 4) in mat4  a_Transform;
layout (location = 8) in int   a_Id;
//uniform mat4 a_Transform; // For non istance render

// Output attributes
// 17 components total(64 min, 128 max on GTX950). One component is one value. vec3 = 3, vec4 = 4, mat4 = 4*4 = 16 components
layout (location = 0)  out vec2 out_UV_Coordinates;
layout (location = 1)  out vec3 out_Normal;
layout (location = 2)  out vec3 out_Vertex;
layout (location = 3)  out mat3 out_TBN_Matrix;
layout (location = 10) out int  out_Id;
layout (location = 11) out vec4 out_FragPosLightSpace;

// Camera uniform buffer
layout(std140, binding = 0) uniform UCamera
{
	Camera u_Camera;
};
// Clip distance uniform buffer
layout(std140, binding = 1) uniform UClipDistance
{
	vec4 u_ClipDistance;
};

uniform mat4 u_LightMatrix;
// Main entry point
void main()
{
	// Set vertex position
	gl_Position 		= u_Camera.ViewProjection * a_Transform *  vec4(a_Position, 1.0);
	// Clip
	gl_ClipDistance[0] 	= dot(u_Camera.View * vec4(a_Position, 1.0), u_ClipDistance);
	// Pass other data to fragment shader
	out_UV_Coordinates 	  = a_UV_Coordinates;
	out_Normal  		  = normalize((a_Transform 	* vec4(a_Normal, 	0.0)).xyz);
	out_Vertex 			  = (a_Transform 	* vec4(a_Position, 	1.0)).xyz;
	out_TBN_Matrix		  = GetTBN_Matrix(a_Transform, a_Normal, a_Tangent);	
	out_Id 				  = a_Id;
	out_FragPosLightSpace = u_LightMatrix * a_Transform * vec4(a_Position, 1.0);
}
// !End of vertex shader
#type fragment
#version 460 core
#include "resources/shaders/General/BillinPhong.glsl"
#include "resources/shaders/General/Structures.glsl"
#include "resources/shaders/General/FragmentMath.glsl"
// Input
layout (location = 0)  in vec2 a_UV_Coordinates;
layout (location = 1)  in vec3 a_Normal;
layout (location = 2)  in vec3 a_Vertex;
layout (location = 3)  in mat3 a_TBN_Matrix;
layout (location = 10) flat in int a_Id; // dont know why should be flat
layout (location = 11) in vec4    a_FragPosLightSpace; // dont know why should be flat
// Textures
layout (binding = 0) uniform sampler2D u_TDiffuse;
layout (binding = 1) uniform sampler2D u_TSpecular;
layout (binding = 2) uniform sampler2D u_TNormal;
layout (binding = 3) uniform sampler2D u_ShadowMap;
// Camera uniform buffer
layout (std140, binding = 0) uniform UCamera
{
	Camera u_Camera;
};
// Direct lights SSBO buffer
layout (std430, binding = 2) restrict readonly buffer UDirectlight
{
	DirectLight u_DirectLight[];
};
// Point lights SSBO buffer
layout (std430, binding = 3) restrict readonly buffer UPointlight
{
	PointLight u_PointLight[];
};
// Spot lights SSBO buffer
layout (std430, binding = 4) restrict readonly buffer USpotlight
{
	SpotLight u_SpotLight[];
};
// Need to pack in SSBO material as well !!
uniform Material          u_Material;
// Subroutines
subroutine vec4 LightingCalculation(vec3 toCameraDirection);
subroutine (LightingCalculation) 
vec4 FlatColor(vec3 toCameraDirection)
{
	// Without normal map and textures
	vec4 Color = vec4(0.0, 0.0, 0.0, 0.0);

	for(int i = 0;i < u_DirectLight.length(); i++)
		Color += BilinPhongDirectLight(a_Normal, u_DirectLight[i], u_Material,           toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)); 
	for(int i = 0;i < u_PointLight.length();  i++)
		Color += BilinPhongPointLight(a_Normal, u_PointLight[i],   u_Material, a_Vertex, toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)); 
	for(int i = 0;i < u_SpotLight.length();  i++)
		Color += BilinPhongSpotLight(a_Normal, u_SpotLight[i],     u_Material, a_Vertex, toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)); 

	return Color;	 
};
subroutine (LightingCalculation)
vec4 BillinPhong(vec3 toCameraDirection) 
{
	vec4 Color      = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 TBN_Normal = Get_TBNNormal(texture(u_TNormal, a_UV_Coordinates).rgb, a_TBN_Matrix);

	for(int i = 0;i < u_DirectLight.length(); i++)
		Color += BilinPhongDirectLight(TBN_Normal, u_DirectLight[i], u_Material,           toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba); 
	for(int i = 0;i < u_PointLight.length();  i++)
		Color += BilinPhongPointLight(TBN_Normal, u_PointLight[i],   u_Material, a_Vertex, toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba);
	for(int i = 0;i < u_SpotLight.length();  i++)
		Color += BilinPhongSpotLight(TBN_Normal, u_SpotLight[i],     u_Material, a_Vertex, toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba);

	return Color;	
}; 
subroutine uniform LightingCalculation u_sLighting;
// Output buffer
layout (location = 0) out vec4 FrameBufferAttachment;
layout (location = 1) out int  Selected;

// Main entry point
void main()
{
	vec3 ToCameraDirection 		= normalize(u_Camera.Position - a_Vertex);
	// Do lighting calculation;
	vec4 Color 					= u_sLighting(ToCameraDirection);
	float shadow = ShadowCalculation(u_ShadowMap, vec3(-10.0f, 150.0f, -10.f), a_Normal, a_Vertex, a_FragPosLightSpace);                      
	FrameBufferAttachment 		= vec4(Color.rgb * (1.0 - shadow) + (u_Material.DiffuseColor * u_Material.Emissive), 1.0);
	Selected 					= a_Id;
}
// !End of fragment shader