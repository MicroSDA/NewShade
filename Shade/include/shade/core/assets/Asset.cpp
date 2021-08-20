#include "shade_pch.h"
#include "Asset.h"
#include "shade/core/assets/AssetManager.h"

shade::Asset::~Asset()
{
    shade::AssetManager::_ImLast(m_AssetData.Attribute("Id").as_string());
    SHADE_CORE_DEBUG("Asset '{0}' has been deleted.", m_AssetData.Attribute("Id").as_string())
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
