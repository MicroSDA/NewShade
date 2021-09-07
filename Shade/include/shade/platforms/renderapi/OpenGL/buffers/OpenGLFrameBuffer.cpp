#include "shade_pch.h"
#include "OpenGLFrameBuffer.h"

namespace shade
{
	namespace util
	{
		static const std::uint32_t s_MaxFrameBufferSize = 8192;

		static GLenum TextureTarget(const bool& isMultiSampled)
		{
			return isMultiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(std::uint32_t& outID, const std::uint32_t& count, const bool& isMultiSampled)
		{
			glCreateTextures(TextureTarget(isMultiSampled), count, &outID);
		}
		static void BindTexture(const bool& isMultiSampled, const std::uint32_t& id)
		{
			glBindTexture(TextureTarget(isMultiSampled), id);
		}
		static void AttachTexture(int buffer, const std::uint32_t& id, const std::int32_t& samples, const GLenum& internalFormat, const GLenum& format, const std::uint32_t& width, const std::uint32_t& height, const std::uint32_t& index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				if(internalFormat == GL_RGBA16F)
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr);
				else
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  7);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glGenerateTextureMipmap(id);
				//glTexImage2D(GL_TEXTURE_2D, 1, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
			
		}

		static void AttachDepthTexture(const std::uint32_t& id, const std::int32_t& samples, const GLenum& format, const GLenum& attachmentType, const std::uint32_t& width, const std::uint32_t& height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(const FrameBuffer::Texture::Format&  format)
		{
			switch (format)
			{
			case FrameBuffer::Texture::Format::DEPTH24STENCIL8:  return true;
			default: return false;
			}
		}
		static GLenum ToOpenGLTextureFormat(const FrameBuffer::Texture::Format& format)
		{
			switch (format)
			{
			case FrameBuffer::Texture::Format::RGBA8:       return GL_RGBA8;
			case FrameBuffer::Texture::Format::RED_INT:		return GL_RED_INTEGER;
			case FrameBuffer::Texture::Format::RGBA10:		return GL_RGB10_A2;
			case FrameBuffer::Texture::Format::RGBA16F:		return GL_RGBA16F;
			default: return 0;
			}
		}
	}
}


shade::OpenGLFrameBuffer::OpenGLFrameBuffer(const Layout& layout):
	m_Layout(layout)
{
	for (auto spec : m_Layout.Attachments.Attachments)
	{
		if (!util::IsDepthFormat(spec.TextureFormat))
			m_AttachmentSpec.emplace_back(spec);
		else
			m_DepthAttachmentSpec = spec;
	}

	Invalidate();
}

shade::OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	glDeleteFramebuffers(1, &m_RenderId);
	glDeleteTextures(m_TextureAttacments.size(), m_TextureAttacments.data());
	glDeleteTextures(1, &m_DepthAttachment);
}

void shade::OpenGLFrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RenderId);
	glViewport(0, 0, m_Layout.Width, m_Layout.Height);
}

void shade::OpenGLFrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shade::OpenGLFrameBuffer::Resize(const std::uint32_t& width, const std::uint32_t& height)
{
	if (width == 0 || height == 0 || width > util::s_MaxFrameBufferSize || height > util::s_MaxFrameBufferSize)
	{
		SHADE_CORE_WARNING("Wrong frame buffer size : {0}, {1}", width, height);
		return;
	}
	m_Layout.Width = width;
	m_Layout.Height = height;

	Invalidate();
}

shade::FrameBuffer::Texture::Data shade::OpenGLFrameBuffer::GetData(const std::uint32_t& attachment, const std::uint32_t& x, const std::uint32_t& y) const
{
	// Only for red integer for now
	if (attachment < m_TextureAttacments.size())
	{
		std::int32_t pixelData;
		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		UnBind();
		return FrameBuffer::Texture::Data(pixelData, -1, -1);
	}
	return FrameBuffer::Texture::Data(-1, -1, -1);
}

std::uint32_t shade::OpenGLFrameBuffer::GetAttachment(const std::uint32_t& index) const
{
	if(index >= m_TextureAttacments.size())
	{
		SHADE_CORE_WARNING("Wrong frame buffer attacment id :'{0}'", index);
		return 0;
	}
	else
	{
		return m_TextureAttacments[index];
	}
}

void shade::OpenGLFrameBuffer::BindTextureAttachment(const std::uint32_t& index, const std::uint32_t& unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, GetAttachment(index));
}

void shade::OpenGLFrameBuffer::_ClearAttachmentInt(const std::uint32_t& attachment, const std::int32_t& clearValue)
{
	if (attachment < 0 || attachment >= m_TextureAttacments.size())
		SHADE_CORE_WARNING("Wrong frame buffer attacment id.");

	auto& spec = m_AttachmentSpec[attachment];
	glClearTexImage(m_TextureAttacments[attachment], 0,
		util::ToOpenGLTextureFormat(spec.TextureFormat), GL_INT, &clearValue);
}

void shade::OpenGLFrameBuffer::_ClearAttachmentFloat(const std::uint32_t& attachment, const float& clearValue)
{
	if (attachment < 0 || attachment >= m_TextureAttacments.size())
		SHADE_CORE_WARNING("Wrong frame buffer attacment id.");

	GLuint clearColor[4] = { clearValue, clearValue, clearValue, clearValue }; // Wery big question

	auto& spec = m_AttachmentSpec[attachment];
	glClearTexImage(m_TextureAttacments[attachment], 0,
		GL_BGRA, GL_UNSIGNED_BYTE, &clearColor);
}


void shade::OpenGLFrameBuffer::Invalidate()
{
	if (m_RenderId)
	{
		glDeleteFramebuffers(1, &m_RenderId);
		glDeleteTextures(m_TextureAttacments.size(), m_TextureAttacments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_TextureAttacments.clear();
		m_DepthAttachment = 0;
	}



	glCreateFramebuffers(1, &m_RenderId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RenderId);

	bool multisample = m_Layout.Samples > 1;

	// Attachments
	if (m_AttachmentSpec.size())
	{
		m_TextureAttacments.resize(m_AttachmentSpec.size());
		util::CreateTextures(*m_TextureAttacments.data(), m_TextureAttacments.size(), multisample);

		for (size_t i = 0; i < m_TextureAttacments.size(); i++)
		{
			util::BindTexture(multisample, m_TextureAttacments[i]);
			switch (m_AttachmentSpec[i].TextureFormat)
			{
			case Texture::Format::RGBA8:
				util::AttachTexture(m_RenderId,m_TextureAttacments[i], m_Layout.Samples, GL_RGBA8, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RGBA10:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, GL_RGB10_A2, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RGBA16F:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, GL_RGBA16F, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RED_INT:
				util::AttachTexture(m_RenderId,m_TextureAttacments[i], m_Layout.Samples, GL_R32I, GL_RED_INTEGER, m_Layout.Width, m_Layout.Height, i);
				break;
			}
		}
	}

	if (m_DepthAttachmentSpec.TextureFormat != Texture::Format::None)
	{
		util::CreateTextures(m_DepthAttachment, 1, multisample );
		util::BindTexture(multisample, m_DepthAttachment);
		switch (m_DepthAttachmentSpec.TextureFormat)
		{
		case Texture::Format::DEPTH24STENCIL8:
			util::AttachDepthTexture(m_DepthAttachment, m_Layout.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Layout.Width, m_Layout.Height);
			break;
		}
	}

	if (m_TextureAttacments.size() > 1)
	{
		if (m_TextureAttacments.size() <= 4)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_TextureAttacments.size(), buffers);
		}
		else
		{
			SHADE_CORE_WARNING("Frame buffer color attachment more then 4 !");
		}

	}
	else if (m_TextureAttacments.empty())
	{
		// Only depth-pass
		glDrawBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SHADE_CORE_WARNING("Frame buffer isnt completed.");
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const shade::FrameBuffer::Layout& shade::OpenGLFrameBuffer::GetLayout() const
{
	return m_Layout;
}
