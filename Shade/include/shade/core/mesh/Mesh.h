#pragma once
#include "shade/core/assets/Asset.h"
#include "shade/core/vertex/Vertex3D.h"
#include "shade/core/image/Texture.h"
#include "shade/core/mesh/Material.h"
#include "shade/core/render/drawable/Drawable.h"
namespace shade
{
	class SHADE_API Mesh : public Asset, public Drawable
	{
	public:
		static Shared<Mesh> Create();
		virtual ~Mesh();
		void AddTexture(const Shared<Texture>& texture);
		const std::vector<Shared<Texture>>& GetTextures();

		virtual void AssetInit() override;

		virtual void LoadFromAssetData(shade::AssetData& data) override;
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	private:
		std::vector<Shared<Texture>>	m_Textures; // Need to move textures to material
	};
}