#include "shade_pch.h"
#include "OpenGLTexture.h"

shade::OpenGLTexture::OpenGLTexture() :
	m_RenderID(0), m_IsInit(false)
{
}

shade::OpenGLTexture::~OpenGLTexture()
{
	glDeleteTextures(1, &m_RenderID);
}

void shade::OpenGLTexture::Bind(const Shared<Shader>& shader, std::uint32_t binding) const
{
	if (strcmp(GetAssetData().Attribute("texture_type").as_string(), "diffuse") == 0)
	{
		shader->SendInt("textures.Samplers[0].Sampler", binding);
		glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(binding));
		glBindTexture(GL_TEXTURE_2D, m_RenderID);
	}
	else if (strcmp(GetAssetData().Attribute("texture_type").as_string(), "specular") == 0)
	{
		shader->SendInt("textures.Samplers[1].Sampler", binding);
		glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(binding));
		glBindTexture(GL_TEXTURE_2D, m_RenderID);
	}
	else if (strcmp(GetAssetData().Attribute("texture_type").as_string(), "normal_map") == 0)
	{
		shader->SendInt("textures.Samplers[2].Sampler", binding);
		glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(binding));
		glBindTexture(GL_TEXTURE_2D, m_RenderID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

std::uint32_t shade::OpenGLTexture::GetRenderID()
{
	return m_RenderID;
}

void shade::OpenGLTexture::AssetInit()
{
	_Init();
}

void shade::OpenGLTexture::_Init()
{
	if (m_ImageData.Data != nullptr)
	{
		GLint  format = 0;
		GLuint blockSize = 0;

		std::string texture_type = GetAssetData().Attribute("texture_type").as_string();
		switch (m_ImageData.Compression)
		{
		case ImageData::DXTCompression::DXT1:
			format = (texture_type == "normal_map") ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT; // noramal in srgb for nanosuit only
			blockSize = 8;
			break;
		case ImageData::DXTCompression::DXT3:
			format = (texture_type == "normal_map") ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case ImageData::DXTCompression::DXT5:
			format = (texture_type == "normal_map") ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		default:
			SHADE_CORE_WARNING("Unsupported texture format in '{0}'.", GetAssetData().Attribute("id").as_string());
			return;
		}

		glGenTextures(1, &m_RenderID);
		glBindTexture(GL_TEXTURE_2D, m_RenderID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  m_ImageData.MipMapCount - 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLuint width = m_ImageData.Width;
		GLuint height = m_ImageData.Height;
		GLuint offset = 0;

		for (GLuint i = 0; i < m_ImageData.MipMapCount; i++)
		{
			if (width == 0 || height == 0)
				continue;

			GLsizei size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, i, format, width, height, 0, size, m_ImageData.Data + offset);
			offset += size;
			width /= 2;
			height /= 2;
		}


		// Anisotropic filtering
		GLfloat _Anisotropic;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &_Anisotropic);
		if (_Anisotropic)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _Anisotropic);

		glBindTexture(GL_TEXTURE_2D, 0);
		m_ImageData.Delete();
		m_IsInit = true;
	}
}
