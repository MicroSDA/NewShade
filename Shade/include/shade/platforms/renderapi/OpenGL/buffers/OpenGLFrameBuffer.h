#pragma once
#include "shade/core/render/buffers/FrameBuffer.h"
#include "shade/core/utils/Log.h"
#include <glad/glad.h>

namespace shade
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const Layout& layout);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void Resize(const std::uint32_t& width, const std::uint32_t& height) override;
		virtual Texture::Data GetData(const std::uint32_t& attachment, const std::uint32_t& x, const std::uint32_t& y) const override;
		virtual std::uint32_t GetAttachment(const std::uint32_t& index) const override;
		virtual std::uint32_t GetDepthAttachment() const;
		virtual void BindAttachmentAsTexture(const std::uint32_t& attachment, const std::uint32_t& unit) const override;
		virtual void BindDepthAsTexture(const std::uint32_t& unit) const override;
		virtual void BindAsImage(const std::uint32_t& attachment, const std::uint32_t& binding, const std::uint32_t& mip, const Texture::Format& format, const Texture::Access& access) override;
	private:
		virtual void _ClearAttachmentInt(const std::uint32_t& attachment, const std::int32_t& clearValue = 0) override;
		virtual void _ClearAttachmentFloat(const std::uint32_t& attachment, const float& clearValue = 0.0f) override;
		void Invalidate();
		GLuint m_RenderId = 0;
		std::vector<Texture>	m_AttachmentSpec;
		Texture					m_DepthAttachmentSpec = Texture::Format::None;
		Layout					m_Layout;
		std::vector<GLuint>		m_TextureAttacments;
		GLuint					m_DepthAttachment = 0;

		// Inherited via FrameBuffer
		virtual const Layout& GetLayout() const override;


		// Inherited via FrameBuffer
	};
}