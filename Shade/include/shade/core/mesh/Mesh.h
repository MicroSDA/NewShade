#pragma once
#include "shade/core/assets/Asset.h"
#include "shade/core/vertex/Vertex3D.h"
#include "shade/core/image/Texture.h"
#include "shade/core/mesh/Material3D.h"

#include "shade/core/render/drawable/Drawable.h"
namespace shade
{
	class SHADE_API Mesh : public Asset, public Drawable
	{
	public:
		virtual ~Mesh();
		virtual void AssetInit() override;
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	private:
		// Asset loading behaviour
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) override;
	};
}