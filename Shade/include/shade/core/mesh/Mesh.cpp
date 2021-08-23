#include "shade_pch.h"
#include "Mesh.h"
#include "shade/core/assets/AssetManager.h"

shade::Shared<shade::Mesh> shade::Mesh::Create()
{
	return CreateShared<Mesh>();
}

shade::Mesh::~Mesh()
{
}

void shade::Mesh::AddTexture(const Shared<Texture>& texture)
{
	m_Textures.push_back(texture);
}

const std::vector<shade::Shared<shade::Texture>>& shade::Mesh::GetTextures()
{
	return m_Textures;
}

void shade::Mesh::AssetInit()
{
	// Empty there
}

void shade::Mesh::LoadFromAssetData(shade::AssetData& data, const shade::AssetData& bundle)
{
	SetAssetData(data);

	for (auto& dependency : bundle.Dependencies())
	{
		const std::string id	= dependency.Attribute("Id").as_string();
		auto asset				= AssetManager::GetAssetData(id);

		if (std::string(asset.Attribute("Type").as_string()) == "Texture")
		{
			AssetManager::Hold<Texture>(id, Asset::State::RemoveIfPosible,
				[this](auto& texture) mutable
				{
					m_Textures.push_back(AssetManager::Receive<Texture>(texture));
				}, dependency);
		}
	}

	Deserialize();
	GenerateHalfExt();
}
bool shade::Mesh::Serialize(std::ostream& stream) const
{
	return false;
}

bool shade::Mesh::Deserialize(std::istream& stream)
{
	return false;
}

bool shade::Mesh::Serialize() const
{
	if (!GetAssetData().Serialize())
		return false;
	else
	{
		std::string id		= GetAssetData().Attribute("Id").as_string();
		std::string path	= GetAssetData().Attribute("Path").as_string();

		if (path.empty())
		{
			SHADE_CORE_WARNING("Failded to create directories for asset '{0}', file path is empty.", id);
			return false;
		}

		// Need to create one varible to save total size of file and place it in header to evade corupted file when reading
		std::ofstream file;
		file.open(path, std::ios::binary);
		if (file.is_open() && file.good())
		{
			shade::util::Binarizer::Write(file, "@s_mesh");
			//shade::util::Binarizer::Write(file, id); // Maby will crash
			shade::util::Binarizer::Write(file, std::uint32_t(GetVertices().size()));
			shade::util::Binarizer::Write(file, std::uint32_t(GetIndices().size()));

			for (auto& vertex : GetVertices())
			{
				shade::util::Binarizer::Write(file, vertex.Position.x);
				shade::util::Binarizer::Write(file, vertex.Position.y);
				shade::util::Binarizer::Write(file, vertex.Position.z);

				shade::util::Binarizer::Write(file, vertex.UV_Coordinates.x);
				shade::util::Binarizer::Write(file, vertex.UV_Coordinates.y);

				shade::util::Binarizer::Write(file, vertex.Normal.x);
				shade::util::Binarizer::Write(file, vertex.Normal.y);
				shade::util::Binarizer::Write(file, vertex.Normal.z);

				shade::util::Binarizer::Write(file, vertex.Tangent.x);
				shade::util::Binarizer::Write(file, vertex.Tangent.y);
				shade::util::Binarizer::Write(file, vertex.Tangent.z);

				//Tangenst later if needed 
			}
			for (auto& index : GetIndices())
			{
				shade::util::Binarizer::Write(file, shade::Indices(index));
			}

			// Material
			shade::util::Binarizer::Write(file, *glm::value_ptr(GetMaterial().GetAmbientColor()), 3); // RGB
			shade::util::Binarizer::Write(file, *glm::value_ptr(GetMaterial().GetDiffuseColor()), 3); // RGB
			shade::util::Binarizer::Write(file, *glm::value_ptr(GetMaterial().GetSpecularColor()), 3); // RGB 
			shade::util::Binarizer::Write(file, *glm::value_ptr(GetMaterial().GetTransparentMask()), 3); // RGB
			shade::util::Binarizer::Write(file, GetMaterial().GetShininess());
			shade::util::Binarizer::Write(file, GetMaterial().GetShininessStrength());

			file.close();
		}
		else
		{
			SHADE_CORE_WARNING("Failed to open file '{0}'.", path + id + ".s_mesh")
				return false;
		}

		for (const auto& texture : m_Textures)
		{
			if (!texture->Serialize())
				return false;
		}

		return true;
	}
}

bool shade::Mesh::Deserialize()
{
	// Now only if it prsenet in asset data list
	std::string id		= GetAssetData().Attribute("Id").as_string();
	std::string path	= GetAssetData().Attribute("Path").as_string();

	std::ifstream file;
	file.open(path, std::ios::binary);

	if (file.is_open() && file.good())
	{
		std::string header;
		shade::util::Binarizer::Read(file, header);
		header.pop_back();
		if (header == "@s_mesh")
		{
			std::uint32_t verticesCount = 0;
			shade::util::Binarizer::Read(file, verticesCount);
			if (verticesCount <= 50000) // limit !
			{
				GetVertices().reserve(verticesCount);

				std::uint32_t indicesCount = 0;
				shade::util::Binarizer::Read(file, indicesCount);
				if (indicesCount <= 50000) // limit !
				{
					GetIndices().reserve(indicesCount);
					for (auto v = 0; v < verticesCount; v++)
					{
						shade::Vertex3D vertex;
						shade::util::Binarizer::Read(file, vertex.Position.x);
						shade::util::Binarizer::Read(file, vertex.Position.y);
						shade::util::Binarizer::Read(file, vertex.Position.z);

						shade::util::Binarizer::Read(file, vertex.UV_Coordinates.x);
						shade::util::Binarizer::Read(file, vertex.UV_Coordinates.y);

						shade::util::Binarizer::Read(file, vertex.Normal.x);
						shade::util::Binarizer::Read(file, vertex.Normal.y);
						shade::util::Binarizer::Read(file, vertex.Normal.z);

						shade::util::Binarizer::Read(file, vertex.Tangent.x);
						shade::util::Binarizer::Read(file, vertex.Tangent.y);
						shade::util::Binarizer::Read(file, vertex.Tangent.z);

						GetVertices().push_back(vertex);
					}
					for (auto i = 0; i < indicesCount; i++)
					{

						Index index;
						shade::util::Binarizer::Read(file, index);
						GetIndices().push_back(index);
					}

					shade::util::Binarizer::Read(file, *glm::value_ptr(GetMaterial().GetAmbientColor()), 3); // RGB
					shade::util::Binarizer::Read(file, *glm::value_ptr(GetMaterial().GetDiffuseColor()), 3); // RGB
					shade::util::Binarizer::Read(file, *glm::value_ptr(GetMaterial().GetSpecularColor()), 3); // RGB 
					shade::util::Binarizer::Read(file, *glm::value_ptr(GetMaterial().GetTransparentMask()), 3); // RGB
					shade::util::Binarizer::Read(file, GetMaterial().GetShininess());
					shade::util::Binarizer::Read(file, GetMaterial().GetShininessStrength());

					return true;
				}
				else
				{
					SHADE_CORE_WARNING("Indices limit exceeded in '{0}'.", path + id + ".s_mesh");
					return false;
				}
			}
			else
			{
				SHADE_CORE_WARNING("Vertices limit exceeded in '{0}'.", path + id + ".s_mesh");
				return false;
			}
		}
		else
		{
			SHADE_CORE_WARNING("Wrong header in '{0}'.", path + id + ".s_mesh");
			return false;
		}

	}
	else
	{
		SHADE_CORE_WARNING("Failed to open file '{0}'.", path + id + ".s_mesh")
			return false;
	}
}

