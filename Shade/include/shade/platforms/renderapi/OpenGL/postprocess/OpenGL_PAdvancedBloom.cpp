#include "shade_pch.h"
#include "OpenGL_PAdvancedBloom.h"

shade::OpenGL_PAdvancedBloom::OpenGL_PAdvancedBloom()
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA8,
		  shade::FrameBuffer::Texture::Format::RGBA8,
		}));

	m_BloomFrameBuffer2 = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		  shade::FrameBuffer::Texture::Format::RGBA8,
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
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	// 
	//m_InputFrameBuffer->Bind();
	m_BloomShader->Bind();
	m_BloomShader->SelectSubrutine("s_Stage", "PreF13", Shader::Type::Compute);
	glBindImageTexture(0, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	
	m_BloomShader->SelectSubrutine("s_Stage", "PreF4", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "DownSample13", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "DownBox", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 4, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "UpBox", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 4, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "BV", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "BH", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "UpTent", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "UpBox", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	m_BloomShader->SelectSubrutine("s_Stage", "Combine", Shader::Type::Compute);
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	/* Поменять буфер формат атачмента, посмотреть как в старой версии стоит офсет на увеличение потому что минусы как то работуют странно*/
	/* Проверить верность тех координат когда идет комбайн, потому что выглядит как не верно*/
	/* / или * 2 похоже не верны */
	/* не хватате одного апскейла + пробрасыать через буфер се надо и не через один походу мип мапы не те стоят + умножение дилени ен забыть на текс коорд*/



	
	//glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
}
