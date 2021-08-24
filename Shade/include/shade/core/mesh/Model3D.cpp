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
    SetAssetData(data);

    for (auto& dependancy : GetAssetData().Dependencies())
    {
        const std::string id    = dependancy.Attribute("Id").as_string();
        auto asset              = AssetManager::GetAssetData(id);

        if (std::string(asset.Attribute("Type").as_string()) == "Mesh")
        {
            AssetManager::Hold<Mesh>(id, Asset::State::RemoveIfPosible,
                [this](auto& mesh) mutable
                {
                    m_Meshes.push_back(AssetManager::Receive<Mesh>(mesh));
                }, dependancy);
        }
    }
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