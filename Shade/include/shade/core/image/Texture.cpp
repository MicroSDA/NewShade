#include "shade_pch.h"
#include "Texture.h"
#include "shade/core/render/RenderAPI.h"
#include "shade/platforms/renderapi/OpenGL/OpenGLTexture.h"

shade::Shared<shade::Texture> shade::Texture::_Create()
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
	m_ImageData = Image::LoadFromStream(stream);

	if (m_ImageData.Data != nullptr)
		return true;
	else
	{
		return false;
	}
}

bool shade::Texture::Serialize() const
{
    return false;
}

bool shade::Texture::Deserialize()
{
	std::string path = GetAssetData().Attribute("Path").as_string();

	if (std::filesystem::path(path).extension().string() == ".dds")
	{
		std::ifstream file;
		file.open(path, std::ios::binary);

		if (file.is_open() && file.good())
		{
			if (!Deserialize(file))
			{
				SHADE_CORE_WARNING("Failed to decode image '{0}'.", path);
				return false;
			}
			else
				return true;
			file.close();
		}
		else
		{
			SHADE_CORE_WARNING("Failed to open file '{0}'.", path);
			return false;
		}
	}
	else
	{
		SHADE_CORE_WARNING("Failed to import image, only dds supported :'{0}'.", path);
		return false;
	}
}

void shade::Texture::LoadDependentAssetsCallback(const shade::AssetData& data, const std::string& id)
{
	return; // Do nothing;
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

