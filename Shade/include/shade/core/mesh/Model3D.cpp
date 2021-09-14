#include "shade_pch.h"
#include "Model3D.h"
#include "shade/core/assets/AssetManager.h"


shade::Shared<shade::Model3D> shade::Model3D::Create()
{
    return CreateShared<Model3D>();
}

const std::vector<shade::Shared<shade::Mesh>>& shade::Model3D::GetMeshes() const
{
    return m_Meshes;
}

shade::Model3D::~Model3D()
{

}

void shade::Model3D::LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle)
{
    auto dependencies = data.Dependencies();
    /* If it's has a dependencies*/
    if (dependencies.size())
    {
        for (auto& dependancy : dependencies)
        {
            const std::string id = dependancy.Attribute("Id").as_string();
            /* Trying to get dependancy as asset */
            auto dependancyAsset = AssetManager::GetAssetData(id);
            /* If dependancy is asset, we can grab all specific metadata*/
            if (strcmp(dependancyAsset.Attribute("Type").as_string(), "Mesh") == 0)
            {
                AssetManager::HoldPrefab<Mesh>(id,
                    [this](auto& mesh) mutable
                    {
                        m_Meshes.push_back(AssetManager::Receive<Mesh>(mesh));

                    }, shade::Asset::Lifetime::Destroy);
            }
        }
    }

    // Model 3d is prefab only
    SetAssetData(data);
}

void shade::Model3D::AssetInit()
{
    // do notnig
}


bool shade::Model3D::Serialize(std::ostream& stream) const
{
    return false;
}

bool shade::Model3D::Deserialize(std::istream& stream)
{
    return false;
}

bool shade::Model3D::Serialize() const
{
    return false;
}

bool shade::Model3D::Deserialize()
{
    return false;
}