#include "shade_pch.h"
#include "Texture.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLTexture.h"

shade::Shared<shade::Texture> shade::Texture::Create()
{
    switch (RenderAPI::GetAPI())
	{
	case RenderAPI::API::None:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	case RenderAPI::API::OpenGL:	return CreateShared<OpenGLTexture>();
	case RenderAPI::API::Vulkan:
		SHADE_CORE_ERROR("Unsupported render API!"); return nullptr;
	default:
		SHADE_CORE_ERROR("Undefined render API!"); return nullptr;
	}
}

bool shade::Texture::Serialize(std::ostream& stream) const
{
    return false;
}

bool shade::Texture::Deserialize(std::istream& stream)
{
    return false;
}

bool shade::Texture::Serialize() const
{
    return false;
}

bool shade::Texture::Deserialize()
{
	//return false; ////!!!!!!!!!!!!
	 // Now only if it prsenet in asset data list
	std::string id = GetAssetData().Attribute("id").as_string();
	std::string path = GetAssetData().Attribute("path").as_string();

	std::ifstream file;
	file.open(path + id + ".dds", std::ios::binary);

	if (file.is_open() && file.good())
	{
		m_ImageData = Image::LoadFromStream(file);
		file.close();
		if (m_ImageData.Data != nullptr)
			return true;
		else
		{
			SHADE_CORE_WARNING("Failed to decode image '{0}'.", path + id + ".dds");
			return false;
		}
	}
	else
	{
		SHADE_CORE_WARNING("Failed to open file '{0}'.", path + id + ".dds");
		return false;
	}
}

shade::Texture::~Texture()
{
	m_ImageData.Delete();
}

const shade::ImageData& shade::Texture::GetImageData() const
{
	return m_ImageData;
}

shade::ImageData& shade::Texture::GetImageData()
{
	return const_cast<ImageData&>(const_cast<const Texture*>(this)->GetImageData());
}

void shade::Texture::LoadFromAssetData(shade::AssetData& data)
{
	SetAssetData(data);
	Deserialize();
}
