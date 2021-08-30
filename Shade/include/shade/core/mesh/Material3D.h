#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include "shade/core/image/Texture.h"

namespace shade
{
	struct Material
	{
		enum class BlendingModel : std::uint32_t
		{
			None,
			Test
		};
		/* Base collors spec*/
		glm::vec3		ColorAmbient;
		glm::vec3		ColorDiffuse;
		glm::vec3		ColorSpecular;
		glm::vec3		ColorEmissive;
		glm::vec3		ColorTransparent;
		BlendingModel	Blending;
		bool			WireFrame;
	};
	struct Material3D : Material
	{
		enum ShadingModel
		{
			Flat, Gouraud, Phong, Billin, Toon, OrenNayer, Minnaert, CookTorance, NonShading, Fresnel
		};

		float			Opacity				= 1.0f;
		float			Shininess			= 1.0f;
		float			ShininessStrength	= 1.0f;
		float			Refracti			= 0.0f;
		ShadingModel    Shading				= ShadingModel::Flat;

		Shared<Texture> TextureDiffuse;
		Shared<Texture> TextureSpecular;
		Shared<Texture> TextureNormals;
		Shared<Texture> TextureRoughness;
		Shared<Texture> TextureAlbedo;
		Shared<Texture> TextureMetallic;
	};
}
