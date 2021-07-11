#pragma once
#include "shade/core/assets/Asset.h"
#include "shade/core/image/Image.h"
#include "shade/core/render/Shader.h"

namespace shade
{
	class Texture : public Asset
	{
	public:
		static Shared<Texture> Create();
		virtual ~Texture();

		virtual void Bind(const Shared<Shader>& shader, std::uint32_t binding = 0) const = 0;
		virtual std::uint32_t GetRenderID() = 0;
		const ImageData& GetImageData() const;
		ImageData& GetImageData();

		virtual void LoadFromAssetData(shade::AssetData& data) override;
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	protected:
		ImageData m_ImageData;
	};
}