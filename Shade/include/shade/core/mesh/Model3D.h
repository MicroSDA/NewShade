#pragma once
#include "shade/core/mesh/Mesh.h"

namespace shade
{
	class SHADE_API Model3D :public Asset
	{
	public:
		virtual ~Model3D();
		static Shared<Model3D> Create();

		const std::vector<Shared<Mesh>>& GetMeshes() const;

		virtual void LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle = AssetData()) override;
		virtual void AssetInit() override;
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	protected:// Temporary
		std::vector<Shared<Mesh>> m_Meshes;
	};
}
