#include "shade_pch.h"
#include "OpenGL_PAdvancedBloom.h"

shade::OpenGL_PAdvancedBloom::OpenGL_PAdvancedBloom()
{

}

void shade::OpenGL_PAdvancedBloom::Process()
{
	std::uint32_t groups[2] = { ceil((float)m_InputFrameBuffer->GetLayout().Width / 16.0f), ceil((float)m_InputFrameBuffer->GetLayout().Height / 16.0f) };

	if (m_BloomFrameBuffer->GetLayout().Width     != m_InputFrameBuffer->GetLayout().Width ||
		m_BloomFrameBuffer->GetLayout().Height    != m_InputFrameBuffer->GetLayout().Height ||
		m_BloomFrameBuffer->GetLayout().MipsCount != m_Scaling)
	{
		m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(
			m_InputFrameBuffer->GetLayout().Width,
			m_InputFrameBuffer->GetLayout().Height,
			{
				shade::FrameBuffer::Texture::Format::RGBA16F,
			},
			m_Scaling));
	}


	m_BloomShader->Bind();
	/* Getting hdr picture */
	m_InputFrameBuffer->BindAsImage(0, 1, 0, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Read);
	m_BloomFrameBuffer->BindAsImage(0, 2, 0, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Write);
	m_BloomShader->SelectSubrutine("s_Stage", "HDR", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(groups[0], groups[1], 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	///////////////////////////////////////

	/* Downsampling */
	for (auto i = 0; i < m_Scaling; i++)
	{
		/* Downsample */
		m_BloomShader->SendInt("u_Lod", i);
		m_BloomFrameBuffer->BindAsTexture(0,  0);
		m_BloomFrameBuffer->BindAsImage(  0,  2, i + 1, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Write);
		m_BloomShader->SelectSubrutine("s_Stage", "Downsample", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(ceil(groups[0] / (i + 1.f)), ceil(groups[1] / (i + 1.f)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		/* Blur */
		m_BloomFrameBuffer->BindAsTexture(0,  0);
		m_BloomFrameBuffer->BindAsImage(  0,  2, i + 1, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Write);
		m_BloomShader->SelectSubrutine("s_Stage", "Blur", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(ceil(groups[0] / (i + 1.f)), ceil(groups[1] / (i + 1.f)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
	/* Upsampling */
	for (auto i = m_Scaling; i > 0; i--)
	{
		m_BloomShader->SendInt("u_Lod", i);
		m_BloomFrameBuffer->BindAsTexture(0,  0);
		m_BloomFrameBuffer->BindAsImage(  0,  2, i - 1, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::ReadWrite);
		m_BloomShader->SelectSubrutine("s_Stage", "Upsample", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(ceil(groups[0] / float(i)), ceil(groups[1] / float(i)), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	/* Combine */
	m_BloomFrameBuffer->BindAsImage(0,  1, 0, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Read);
	m_OutputFrameBuffer->BindAsImage(0, 2, 0, FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Write);
	m_BloomShader->SelectSubrutine("s_Stage", "Combine", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(groups[0], groups[1], 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
