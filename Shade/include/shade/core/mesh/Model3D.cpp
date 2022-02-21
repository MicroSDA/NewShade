#include "shade_pch.h"
#include "Model3D.h"
#include "shade/core/assets/AssetManager.h"

const std::vector<shade::Shared<shade::Mesh>>& shade::Model3D::GetMeshes() const
{
    return m_Meshes;
}

shade::Model3D::~Model3D()
{

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
    for (auto& mesh : m_Meshes)
    {
        mesh->GetAssetData().Attribute("Path").set_value(this->GetAssetData().Attribute("Path").as_string());
        mesh->Serialize();
    }
    
    /* FROM UP TO DOWN ASSET DATA MATHCHING */
    GetAssetData().Serialize();
    return false;
}

bool shade::Model3D::Deserialize()
{
    return false;
}

void shade::Model3D::LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id)
{
    if (strcmp(data.Attribute("Type").as_string(), "Mesh") == 0)
    {
        AssetManager::HoldPrefab<Mesh>(id,
            [this](auto& mesh) mutable
            {
                m_Meshes.push_back(AssetManager::Receive<Mesh>(mesh));

            }, shade::Asset::Lifetime::Destroy);
    }
}
