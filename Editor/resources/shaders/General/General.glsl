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
// Output attributes
// 17 components total(64 min, 128 max on GTX950). One component is one value. vec3 = 3, vec4 = 4, mat4 = 4*4 = 16 components
layout (location = 0)  out vec2 out_UV_Coordinates;
layout (location = 1)  out vec3 out_Normal;
layout (location = 2)  out vec3 out_Vertex;
layout (location = 3)  out mat3 out_TBN_Matrix;
//layout (location = 7)  out int  out_Id;
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
layout (std430, binding = 2) restrict readonly buffer UDirectlight
{
	DirectLight u_DirectLight[];
};
// Main entry point
void main()
{
	// Set vertex position
	gl_Position 		= u_Camera.ViewProjection * a_Transform *  vec4(a_Position, 1.0);
	// Clip
	//gl_ClipDistance[0] 	= dot(u_Camera.View * vec4(a_Position, 1.0), u_ClipDistance);
	// Pass other data to fragment shader
	out_UV_Coordinates 	  = a_UV_Coordinates;
	out_Normal  		  = normalize((a_Transform 	* vec4(a_Normal, 	0.0)).xyz);
	out_Vertex 			  = vec3(a_Transform * vec4(a_Position, 	1.0));
	out_TBN_Matrix		  = GetTBN_Matrix(a_Transform, a_Normal, a_Tangent);
}
// !End of vertex shader
#type fragment
#version 460 core
/* Include in reverse order*/
#include "resources/shaders/General/BillinPhong.glsl"
#include "resources/shaders/General/Effects/CSM.glsl"
#include "resources/shaders/General/Structures.glsl"
#include "resources/shaders/General/FragmentMath.glsl"
// Input
layout (location = 0)  in vec2 a_UV_Coordinates;
layout (location = 1)  in vec3 a_Normal;
layout (location = 2)  in vec3 a_Vertex;
layout (location = 3)  in mat3 a_TBN_Matrix;
//layout (location = 7)  flat in int a_Id; // dont know why should be flat
// Textures
layout (binding = 0) uniform sampler2D 		u_TDiffuse;
layout (binding = 1) uniform sampler2D 		u_TSpecular;
layout (binding = 2) uniform sampler2D 		u_TNormal;
layout (binding = 3) uniform sampler2DArray u_TDirectLightShadowMap;
//layout (binding = 4) uniform sampler2D      u_TSpotLightShadowMap;
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
// Shadow map cascades
layout (std430, binding = 5) restrict readonly buffer UDirectlightCascade
{
	DirectLightCascade u_DirectLightCascade[];
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

	/*for(int i = 0;i < u_DirectLight.length(); i++)
		Color += BilinPhongDirectLight(a_Normal, u_DirectLight[i], u_Material,           toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), 1.0); 
	for(int i = 0;i < u_PointLight.length();  i++)
		Color += BilinPhongPointLight(a_Normal, u_PointLight[i],   u_Material, a_Vertex, toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)); 
	for(int i = 0;i < u_SpotLight.length();  i++)
		Color += BilinPhongSpotLight(a_Normal, u_SpotLight[i],     u_Material, a_Vertex, toCameraDirection, vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0)); */

	return Color;	 
};
subroutine (LightingCalculation)
vec4 BillinPhong(vec3 toCameraDirection) 
{
	vec4 Color      = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 TBN_Normal = Get_TBNNormal(texture(u_TNormal, a_UV_Coordinates).rgb, a_TBN_Matrix);
	for(int i = 0;i < u_DirectLight.length(); i++)
		{
			vec4    FragPosVeiwSpace    = u_Camera.View * vec4(a_Vertex, 1.0);
    		float   Depth               = abs(FragPosVeiwSpace.z);
    		int     CascadeLayer        = 0;
			for(int i = 0; i < u_DirectLightCascade.length(); i++)
			{
				if(Depth <= u_DirectLightCascade[i].SplitDistance)
				{
					CascadeLayer = i;	break;
				}
				else if(CascadeLayer == 0)
						CascadeLayer = u_DirectLightCascade.length() - 1; 
			}
			float Shadow  = 1.0 - CSM_DirectLight(u_TDirectLightShadowMap,
												  u_DirectLightCascade[CascadeLayer].ViewMatrix,
												  CascadeLayer,
												  u_DirectLightCascade.length(),
												  u_DirectLightCascade[i].SplitDistance,
												  u_Camera.View,
												  a_Vertex,
												  a_Normal,
												  u_DirectLight[i].Direction);	

			if(CascadeLayer == 0)		
				Color += vec4(0.2, 0.2, 0, 0);
			if(CascadeLayer == 1)
				Color += vec4(0.0, 0.2, 0, 0);
			if(CascadeLayer == 2)
				Color += vec4(0.0, 0.0, 0.2, 0);
			if(CascadeLayer == 3)
				Color += vec4(0.2, 0.0, 0.0, 0);

			Color += BilinPhongDirectLight(TBN_Normal, u_DirectLight[i], u_Material, toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba, Shadow); 
		}
	for(int i = 0;i < u_PointLight.length();  i++)
		Color += BilinPhongPointLight(TBN_Normal, u_PointLight[i],   u_Material, a_Vertex, toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba, 1.0);
	for(int i = 0;i < u_SpotLight.length();  i++)
	{
		/*float Shadow  = 1.0 - SM_SpotLight(u_TSpotLightShadowMap,
												  u_SpotLignViewMatrix[0],
												  u_Camera.View,
												  a_Vertex,
												  a_Normal,
												  u_SpotLight[i].Position);*/

		Color += BilinPhongSpotLight(TBN_Normal, u_SpotLight[i],  u_Material, a_Vertex, toCameraDirection, texture(u_TDiffuse, a_UV_Coordinates).rgba, texture(u_TSpecular, a_UV_Coordinates).rgba, 1.0);
	}
	return Color;	
}; 
subroutine uniform LightingCalculation u_sLighting;
// Output buffer
layout (location = 0) out vec4 FrameBufferAttachment;
// Main entry point
void main()
{
	vec3 ToCameraDirection 		= normalize(u_Camera.Position - a_Vertex);
	// Do lighting calculation;
	vec4 Color 					= u_sLighting(ToCameraDirection);      
	FrameBufferAttachment 		= vec4(Color.rgb  + (u_Material.DiffuseColor * u_Material.Emissive), 1.0);
}
// !End of fragment shader