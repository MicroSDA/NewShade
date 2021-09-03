#include "shade_pch.h"
#include "OpenGL_PBloom.h"

shade::OpenGL_PBloom::OpenGL_PBloom()
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA8,
		  shade::FrameBuffer::Texture::Format::RGBA8,
		}));
}

void shade::OpenGL_PBloom::Process()
{
	std::uint32_t x = ceil((float)m_InputFrameBuffer->GetLayout().Width / 16.0f);
	std::uint32_t y = ceil((float)m_InputFrameBuffer->GetLayout().Height / 16.0f);

	//_CalculateGaussianCoef(m_Data);
	m_GaussianUniformBuffer->SetData(&m_Data, sizeof(PPBloom::ShaderData), 0);

	if (m_BloomFrameBuffer->GetLayout().Width != m_InputFrameBuffer->GetLayout().Width ||
		m_BloomFrameBuffer->GetLayout().Height != m_InputFrameBuffer->GetLayout().Height)
	{
		m_BloomFrameBuffer->Resize(m_InputFrameBuffer->GetLayout().Width, m_InputFrameBuffer->GetLayout().Height);
	}

	
	glMemoryBarrier(GL_ALL_BARRIER_BITS); // I dont know about this

	//glViewport(0, 0, 50, 50);
	m_BloomShader->Bind();
	m_BloomShader->SelectSubrutine("u_Blur",  "Horizontal", shade::Shader::Type::Compute);
	m_BloomShader->SelectSubrutine("u_Stage", "GetHDR", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE,  0, GL_READ_ONLY,  GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE,  0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("u_Blur", "Horizontal", shade::Shader::Type::Compute);
	m_BloomShader->SelectSubrutine("u_Stage", "ProcessBlur", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->DispatchCompute(x, y, 1);
	

	m_BloomShader->SelectSubrutine("u_Blur", "Vertical", shade::Shader::Type::Compute);
	m_BloomShader->SelectSubrutine("u_Stage", "ProcessBlur", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	m_BloomShader->DispatchCompute(x, y, 1);
	

	// Combine stage
	m_BloomShader->SelectSubrutine("u_Blur", "Vertical", shade::Shader::Type::Compute);
	m_BloomShader->SelectSubrutine("u_Stage", "Combine", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0),   0, GL_FALSE,  0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_OutputFrameBuffer->GetAttachment(0),  0, GL_FALSE,  0, GL_READ_WRITE, GL_RGBA8);
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->UnBind();
}
