#include "shade_pch.h"
#include "OpenGL_PAdvancedBloom.h"

shade::OpenGL_PAdvancedBloom::OpenGL_PAdvancedBloom()
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		  shade::FrameBuffer::Texture::Format::RGBA16F,
		}));

	m_BloomFrameBuffer2 = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		  shade::FrameBuffer::Texture::Format::RGBA16F,
		}));
}

void shade::OpenGL_PAdvancedBloom::Process()
{
	std::uint32_t x = ceil((float)m_InputFrameBuffer->GetLayout().Width / 16.0f);
	std::uint32_t y = ceil((float)m_InputFrameBuffer->GetLayout().Height / 16.0f);

	//_CalculateGaussianCoef(m_Data);
	//m_GaussianUniformBuffer->SetData(&m_Data, sizeof(PPAdvancedBloom::ShaderData), 0);

	if (m_BloomFrameBuffer->GetLayout().Width  != m_InputFrameBuffer->GetLayout().Width ||
		m_BloomFrameBuffer->GetLayout().Height != m_InputFrameBuffer->GetLayout().Height)
	{
		m_BloomFrameBuffer->Resize(m_InputFrameBuffer->GetLayout().Width, m_InputFrameBuffer->GetLayout().Height);
	}

	//m_InputFrameBuffer->Bind();
	m_BloomShader->Bind();

	m_BloomShader->SelectSubrutine("u_Stage", "HDR", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->DispatchCompute(x, y, 1);


	auto count = 7;

	for (auto i = 0; i < count - 1; i++)
	{

		m_BloomShader->SelectSubrutine("u_Stage", "DownsampleBox", shade::Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		glBindImageTexture(2, m_BloomFrameBuffer->GetAttachment(0),  i,     GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
		glBindImageTexture(3, m_BloomFrameBuffer->GetAttachment(0),  i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		m_BloomShader->DispatchCompute(x, y, 1);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // I dont know about this
	}

	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // I dont know about this

	for (auto i = count - 1; i > 0; i--)
	{

		m_BloomShader->SelectSubrutine("u_Stage", "UpsempleTent", shade::Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		glBindImageTexture(2, m_BloomFrameBuffer->GetAttachment(0), i,     GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA16F);
		glBindImageTexture(3, m_BloomFrameBuffer->GetAttachment(0), i - 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		m_BloomShader->DispatchCompute(x, y, 1);

		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // I dont know about this
	}


	m_BloomShader->SelectSubrutine("u_Stage", "BlurV", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(3, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("u_Stage", "BlurH", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(3, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("u_Stage", "Combine", shade::Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	glBindImageTexture(3, m_BloomFrameBuffer->GetAttachment(0),  0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA16F);
	glBindImageTexture(1, m_InputFrameBuffer->GetAttachment(0),  0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	m_BloomShader->DispatchCompute(x, y, 1);

}
