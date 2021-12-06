#pragma once
#include "shade/core/assets/Asset.h"
#include "shade/core/image/Image.h"
#include "shade/core/render/Shader.h"

namespace shade
{
	class SHADE_API Texture : public Asset
	{
	public:
		// Make own Create function behaviour
		template<typename>
		inline static Shared<Texture> Create() { return _Create(); }

		virtual ~Texture();
		virtual void Bind(std::uint32_t binding = 0) const = 0;
		virtual std::uint32_t GetRenderID() = 0;
		const ImageData& GetImageData() const;
		ImageData& GetImageData();
		virtual bool Serialize(std::ostream& stream) const override;
		virtual bool Deserialize(std::istream& stream) override;
		virtual bool Serialize() const override;
		virtual bool Deserialize() override;
	protected:
		ImageData m_ImageData;
	private:
		// Asset loading behaviour
		virtual void LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id) override;
		static Shared<Texture> _Create();
	};
}