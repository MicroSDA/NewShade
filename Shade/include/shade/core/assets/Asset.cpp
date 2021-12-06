#include "shade_pch.h"
#include "Asset.h"
#include "shade/core/assets/AssetManager.h"

shade::Asset::Asset() 
    :m_IsPrefab(false)
{
}

shade::Asset::~Asset()
{
    shade::AssetManager::_ImLast(m_AssetData.Attribute("Id").as_string(), m_IsPrefab);
    SHADE_CORE_DEBUG("Asset '{0}' has been deleted.", m_AssetData.Attribute("Id").as_string())
}

void shade::Asset::_LoadFromAssetData(shade::AssetData& data)
{
	/* If prefab has dependencies*/
	auto dependencies = data.Dependencies();
	if (dependencies.size())
	{
		for (auto& dependancy : dependencies)
		{
			const std::string id = dependancy.Attribute("Id").as_string();
			/* Trying to get dependancy as asset */
			auto dependancyAsset = AssetManager::GetAssetData(id);
			/* If dependancy is asset, we can grab all specific metadata*/
			LoadDependentAssetsCallback(dependancyAsset, id);
		}
	}

	/* Trying to get prefab as asset*/
	auto asset = AssetManager::GetAssetData(data.Attribute("Id").as_string());
	/* If current prefab is asset as well */
	if (asset.IsValid())
	{
		/* Set asset data as asset*/
		SetAssetData(asset);
		Deserialize();
	}
	else
	{
		/* Set asset data as prefab only */
		SetAssetData(data);
	}
}

void shade::Asset::SetAssetData(shade::AssetData& data)
{
    m_AssetData = data;
}

shade::AssetData& shade::Asset::GetAssetData()
{
    return const_cast<shade::AssetData&>(const_cast<const shade::Asset*>(this)->GetAssetData());
}

const shade::AssetData& shade::Asset::GetAssetData() const
{
    return m_AssetData;
}

const bool& shade::Asset::IsPrefab() const
{
    return m_IsPrefab;
}

void shade::Asset::_SetPrefab(const bool& isPrefab)
{
    m_IsPrefab = isPrefab;
}
