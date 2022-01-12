#include "shade_pch.h"
#include "OpenGL_PColorCorrection.h"

shade::OpenGL_PColorCorrection::OpenGL_PColorCorrection()
{
}

void shade::OpenGL_PColorCorrection::Process()
{
	std::uint32_t groups[2] = { ceil((float)m_InputFrameBuffer->GetLayout().Width / 16.0f), ceil((float)m_InputFrameBuffer->GetLayout().Height / 16.0f) };

	m_Shader->Bind();
	m_Shader->SendFloat("u_Settings.Gamma",    m_Gamma);
	m_Shader->SendFloat("u_Settings.Exposure", m_Exposure);

	/* Getting hdr picture */
	m_InputFrameBuffer->BindAsImage(0,  0, 0,  FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Read);
	m_OutputFrameBuffer->BindAsImage(0, 1, 0,  FrameBuffer::Texture::Format::RGBA16F, FrameBuffer::Texture::Access::Write);
	m_Shader->DispatchCompute(groups[0], groups[1], 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
