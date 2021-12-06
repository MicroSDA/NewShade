#include "shade_pch.h"
#include "Sprite.h"
#include "shade/core/assets/AssetManager.h"

shade::Sprite::~Sprite()
{
}

void shade::Sprite::AssetInit()
{
}

bool shade::Sprite::Serialize(std::ostream& stream) const
{
    return false;
}

bool shade::Sprite::Deserialize(std::istream& stream)
{
    return false;
}

bool shade::Sprite::Serialize() const
{
    return false;
}

bool shade::Sprite::Deserialize()
{
    return false;
}

const shade::Shared<shade::Texture>& shade::Sprite::GetTexture()
{
    return m_Texture;
}

void shade::Sprite::LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id)
{
	if (strcmp(data.Attribute("Type").as_string(), "Texture") == 0)
	{
		AssetManager::HoldAsset<Texture>(id,
			[this](auto& texture) mutable
			{
				m_Texture = AssetManager::Receive<Texture>(texture);

			}, shade::Asset::Lifetime::Destroy);
	}
}
