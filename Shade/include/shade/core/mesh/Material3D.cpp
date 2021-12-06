#include "shade_pch.h"
#include "Material3D.h"
#include "shade/core/assets/AssetManager.h"

bool shade::Material3D::Serialize(std::ostream& stream) const
{
	return false;
}

bool shade::Material3D::Deserialize(std::istream& stream)
{
	return false;
}

bool shade::Material3D::Serialize() const
{
	// Asset data serialize !

	std::string id   = GetAssetData().Attribute("Id").as_string();
	std::string path = GetAssetData().Attribute("Path").as_string();

	if (path.empty())
	{
		SHADE_CORE_WARNING("Failded to create directories for asset '{0}', file path is empty.", id);
		return false;
	}

	std::ofstream file;
	file.open(path, std::ios::binary);
	if (file.is_open() && file.good())
	{
		/* Header */
		shade::util::Binarizer::Write(file, "@s_mat");
		/* Base colors */
		shade::util::Binarizer::Write(file, *glm::value_ptr(ColorAmbient),		3); // RGB;
		shade::util::Binarizer::Write(file, *glm::value_ptr(ColorDiffuse),		3); // RGB;
		shade::util::Binarizer::Write(file, *glm::value_ptr(ColorSpecular),		3); // RGB;
		shade::util::Binarizer::Write(file, *glm::value_ptr(ColorTransparent),	3); // RGB;

		shade::util::Binarizer::Write(file, Blend); // Probably need cast to uint32 directly
		shade::util::Binarizer::Write(file, WireFrame); 
		shade::util::Binarizer::Write(file, Emmisive);
		shade::util::Binarizer::Write(file, Opacity); 
		shade::util::Binarizer::Write(file, Shininess);
		shade::util::Binarizer::Write(file, ShininessStrength);
		shade::util::Binarizer::Write(file, Refracti);
		shade::util::Binarizer::Write(file, Shading); // Probably need cast to uint32 directly
		
		file.close();
		return true;
	}
	return false;
}

bool shade::Material3D::Deserialize()
{
	// Now only if it prsenet in asset data list
	std::string id   = GetAssetData().Attribute("Id").as_string();
	std::string path = GetAssetData().Attribute("Path").as_string();

	std::ifstream file;
	file.open(path, std::ios::binary);
	if (file.is_open() && file.good())
	{
		std::string header;
		shade::util::Binarizer::Read(file, header);
		header.pop_back();
		if (header == "@s_mat")
		{
			shade::util::Binarizer::Read(file, *glm::value_ptr(ColorAmbient),		3); // RGB;
			shade::util::Binarizer::Read(file, *glm::value_ptr(ColorDiffuse),		3); // RGB;
			shade::util::Binarizer::Read(file, *glm::value_ptr(ColorSpecular),		3); // RGB;
			shade::util::Binarizer::Read(file, *glm::value_ptr(ColorTransparent),	3); // RGB;

			shade::util::Binarizer::Read(file, Blend); // Probably need cast to uint32 directly
			shade::util::Binarizer::Read(file, WireFrame);
			shade::util::Binarizer::Read(file, Emmisive);
			shade::util::Binarizer::Read(file, Opacity);
			shade::util::Binarizer::Read(file, Shininess);
			shade::util::Binarizer::Read(file, ShininessStrength);
			shade::util::Binarizer::Read(file, Refracti);
			shade::util::Binarizer::Read(file, Shading); // Probably need cast to uint32 directly

			return true;
		}
	}

	return false;
}

void shade::Material3D::AssetInit()
{
}

void shade::Material3D::LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id)
{
	if (strcmp(data.Attribute("Type").as_string(), "Texture") == 0)
	{
		AssetManager::HoldAsset<Texture>(id,
			[this](auto& texture) mutable
			{
				Shared<Texture> _texture = AssetManager::Receive<Texture>(texture);

				if (strcmp(_texture->GetAssetData().Attribute("TextureType").as_string(), "Diffuse") == 0)
					TextureDiffuse = _texture;
				if (strcmp(_texture->GetAssetData().Attribute("TextureType").as_string(), "Specular") == 0)
					TextureSpecular = _texture;
				if (strcmp(_texture->GetAssetData().Attribute("TextureType").as_string(), "Normal") == 0)
					TextureNormals = _texture;

			}, shade::Asset::Lifetime::Destroy);
	}
}
