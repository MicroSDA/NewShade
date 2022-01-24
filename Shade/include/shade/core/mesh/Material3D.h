#pragma once
#include "shade/config/API.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shade/core/image/Texture.h"
#include "shade/core/serializing/Serializable.h"
#include "shade/core/assets/Asset.h"

namespace shade
{
	class SHADE_API Material3D : public Asset
	{
	public:
		virtual ~Material3D() = default;
	public:
		struct Blending
		{
			enum class Factor : std::int32_t
			{
				/* OpenGl native values*/
				None = -1, Zero = 0, One = 1,
				SrcColor		= 0x0300, OneMinusSrcColor		= 0x0301, SrcAlpha		= 0x0302, OneMinusSrcAlpha		= 0x0303,
				DstColor		= 0x0306, OneMinusDstColor		= 0x3007, DstAlpha		= 0x0304, OneMinusDstAlpha		= 0x0305,
				ConstantColor	= 0x8001, OneMinusConstantColor = 0x8002, ConstantAlpha = 0x8003, OneMinusConstantAlpha = 0x8004
			};

			Factor SFactor, DFactor;
		};
		/* Base collors spec*/
		glm::vec3				ColorAmbient		= glm::vec3(0.f);
		glm::vec3				ColorDiffuse		= glm::vec3(1.f);
		glm::vec3				ColorSpecular		= glm::vec3(1.f);
		glm::vec3				ColorTransparent	= glm::vec3(-1.f); // Miuse one, so defualt transparent color doesnt exist

		Material3D::Blending	Blend				= Material3D::Blending { Material3D::Blending::Factor::None, Material3D::Blending::Factor::None };
		bool					WireFrame			= false;

		enum class ShadingModel : std::uint32_t
		{
			NonShading, Flat, Gouraud, Phong, Billin, Toon, OrenNayer, Minnaert, CookTorance, Fresnel
		};

		float			Emmisive			= 0.0f;
		float			Opacity				= 1.0f;
		float			Shininess			= 1.0f;
		float			ShininessStrength	= 1.0f;
		float			Refracti			= 0.0f;
		ShadingModel    Shading				= ShadingModel::NonShading;

		Shared<Texture> TextureDiffuse;
		Shared<Texture> TextureSpecular;
		Shared<Texture> TextureNormals;
		Shared<Texture> TextureRoughness;
		Shared<Texture> TextureAlbedo;
		Shared<Texture> TextureMetallic;

		// Inherited via Serializable
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
		// Inherited via Asset
		// TODO: Recreate function and set as like PostLoadCallback
		virtual void AssetInit() override;
	private:
		// Asset loading behaviour
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) override;
	};
}
