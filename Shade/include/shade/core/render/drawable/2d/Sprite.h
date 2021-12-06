#pragma once

#include "shade/config/API.h"
#include "shade/core/image/Texture.h"

namespace shade
{
	class SHADE_API Sprite : public Asset
	{
	public:
		virtual ~Sprite();
		virtual void AssetInit() override;
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	public:
		const Shared<Texture>& GetTexture();
	private:
		// Asset loading behaviour
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) override;
		Shared<Texture> m_Texture;
	};
}