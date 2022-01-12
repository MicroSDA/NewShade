#include "shade_pch.h"
#include "OpenGLFrameBuffer.h"

namespace shade
{
	namespace util
	{
		static const std::uint32_t s_MaxFrameBufferSize = 8192;

		static GLenum TextureTarget(const std::uint32_t& target)
		{
			if (target == 0)
				return GL_TEXTURE_2D;
			else if (target == 1)
				return GL_TEXTURE_2D_MULTISAMPLE;
			else if (target == 2)
				return GL_TEXTURE_2D_ARRAY;
		}

		static void CreateTextures(std::uint32_t& outID, const std::uint32_t& count, const std::uint32_t& target)
		{
			glCreateTextures(TextureTarget(target), count, &outID);
		}
		static void BindTexture(const std::uint32_t& target, const std::uint32_t& id)
		{
			glBindTexture(TextureTarget(target), id);
		}
		static void AttachTexture(int buffer, const std::uint32_t& id, const std::int32_t& samples, const std::int32_t& layers, const std::uint32_t& mipsCount, const GLenum& internalFormat, const GLenum& format, const std::uint32_t& width, const std::uint32_t& height, const std::uint32_t& index)
		{
			/* TODO layers !*/
			if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(1), id, 0);
			}
			else
			{
				if (internalFormat == GL_RGBA16F)
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr);
				else
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				if (mipsCount)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipsCount);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glGenerateTextureMipmap(id);

			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(0), id, 0);

		}

		static void AttachDepthTextureLayerd(const std::uint32_t& id, const std::int32_t& layers, const GLenum& format, const GLenum& attachmentType, const std::uint32_t& width, const std::uint32_t& height)
		{
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, layers, 0, attachmentType, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		}
		static void AttachDepthTexture(const std::uint32_t& id, const std::int32_t& samples, const std::int32_t& layers,  const GLenum& format, const GLenum& attachmentType, const std::uint32_t& width, const std::uint32_t& height)
		{
			if (layers > 1)
			{
				glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, layers, 0, attachmentType, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
			}
			else if (samples > 1)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(1), id, 0);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(0), id, 0);
			}
		}

		static bool IsDepthFormat(const FrameBuffer::Texture::Format& format)
		{
			switch (format)
			{
			case FrameBuffer::Texture::Format::DEPTH24STENCIL8: 
				return true;
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


shade::OpenGLFrameBuffer::OpenGLFrameBuffer(const Layout& layout) :
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_RenderId);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return FrameBuffer::Texture::Data(pixelData, -1, -1);
	}
	return FrameBuffer::Texture::Data(-1, -1, -1);
}

std::uint32_t shade::OpenGLFrameBuffer::GetAttachment(const std::uint32_t& index) const
{
	if (index >= m_TextureAttacments.size())
	{
		SHADE_CORE_WARNING("Wrong frame buffer attacment id :'{0}'", index);
		return 0;
	}
	else
	{
		return m_TextureAttacments[index];
	}
}

std::uint32_t shade::OpenGLFrameBuffer::GetDepthAttachment() const
{
	if (m_DepthAttachment != 0)
		return m_DepthAttachment;

	SHADE_CORE_WARNING("Framebuffer doesnt have depth attachment");
	return 0;
}

void shade::OpenGLFrameBuffer::_ClearAttachmentInt(const std::uint32_t& attachment, const int& clearValue)
{
	if (attachment < 0 || attachment >= m_TextureAttacments.size())
		SHADE_CORE_WARNING("Wrong frame buffer attacment id.");

	glBindFramebuffer(GL_FRAMEBUFFER, m_RenderId);
	glViewport(0, 0, m_Layout.Width, m_Layout.Height); // ?? 
	auto& spec = m_AttachmentSpec[attachment];
	glClearTexImage(m_TextureAttacments[attachment], 0,
		util::ToOpenGLTextureFormat(spec.TextureFormat), GL_INT, &clearValue);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shade::OpenGLFrameBuffer::_ClearAttachmentFloat(const std::uint32_t& attachment, const float& clearValue)
{
	if (attachment < 0 || attachment >= m_TextureAttacments.size())
		SHADE_CORE_WARNING("Wrong frame buffer attacment id.");

	GLuint clearColor[4] = { clearValue, clearValue, clearValue, clearValue }; // Wery big question

	glBindFramebuffer(GL_FRAMEBUFFER, m_RenderId);
	auto& spec = m_AttachmentSpec[attachment];
	glClearTexImage(m_TextureAttacments[attachment], 0,
		GL_BGRA, GL_UNSIGNED_BYTE, &clearColor);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	/* Set target */
	std::uint32_t target = 0;

	if (m_Layout.Samples > 1)
		target = 1;
	else if (m_Layout.Layers > 1)
		target = 2;
	
	// Attachments
	if (m_DepthAttachmentSpec.TextureFormat != Texture::Format::None)
	{
		switch (m_DepthAttachmentSpec.TextureFormat)
		{
		case Texture::Format::DEPTH24STENCIL8:
			util::CreateTextures(m_DepthAttachment, 1, target);
			util::BindTexture(target, m_DepthAttachment);

			if (m_Layout.Layers > 1)
			{
				util::AttachDepthTexture(m_DepthAttachment, m_Layout.Samples, m_Layout.Layers, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, m_Layout.Width, m_Layout.Height);
			}
			else
			{	
				util::AttachDepthTexture(m_DepthAttachment, m_Layout.Samples, m_Layout.Layers, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Layout.Width, m_Layout.Height);
			}
			break;
		}
	}

	if (m_AttachmentSpec.size())
	{
		m_TextureAttacments.resize(m_AttachmentSpec.size());
		util::CreateTextures(*m_TextureAttacments.data(), m_TextureAttacments.size(), target);

		for (size_t i = 0; i < m_TextureAttacments.size(); i++)
		{
			util::BindTexture(target, m_TextureAttacments[i]);
			switch (m_AttachmentSpec[i].TextureFormat)
			{
			case Texture::Format::RGBA8:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, m_Layout.Layers, m_Layout.MipsCount, GL_RGBA8, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RGBA10:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, m_Layout.Layers, m_Layout.MipsCount, GL_RGB10_A2, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RGBA16F:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, m_Layout.Layers, m_Layout.MipsCount, GL_RGBA16F, GL_RGBA, m_Layout.Width, m_Layout.Height, i);
				break;
			case Texture::Format::RED_INT:
				util::AttachTexture(m_RenderId, m_TextureAttacments[i], m_Layout.Samples, m_Layout.Layers, m_Layout.MipsCount, GL_R32I, GL_RED_INTEGER, m_Layout.Width, m_Layout.Height, i);
				break;
			}
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
		//glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SHADE_CORE_WARNING("Frame buffer isn't completed.");
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const shade::FrameBuffer::Layout& shade::OpenGLFrameBuffer::GetLayout() const
{
	return m_Layout;
}

void shade::OpenGLFrameBuffer::BindAttachmentAsTexture(const std::uint32_t& attachment, const std::uint32_t& unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, GetAttachment(attachment));
}

void shade::OpenGLFrameBuffer::BindDepthAsTexture(const std::uint32_t& unit) const
{
	switch (m_Layout.Layers)
	{
	case 1:
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, GetDepthAttachment());
		break;
	default:
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, GetDepthAttachment());
		break;
	}
}

void shade::OpenGLFrameBuffer::BindAsImage(const std::uint32_t& attachment, const std::uint32_t& binding, const std::uint32_t& mip, const Texture::Format& format, const Texture::Access& access)
{
	glBindImageTexture(binding, GetAttachment(attachment), mip, GL_FALSE, 0, static_cast<GLenum>(access), util::ToOpenGLTextureFormat(format));
}

void shade::OpenGLFrameBuffer::Clear(shade::AttachmentClear clear)
{
	Bind();
	glClear(static_cast<GLbitfield>(clear));
}

void shade::OpenGLFrameBuffer::FlushToScreen()
{
	/* Nedd to resilve depth issue !*/
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RenderId);
	glDrawBuffer(GL_FRONT);
	glBlitFramebuffer(0, 0, m_Layout.Width, m_Layout.Height, 0, 0, m_Layout.Width, m_Layout.Height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
}
