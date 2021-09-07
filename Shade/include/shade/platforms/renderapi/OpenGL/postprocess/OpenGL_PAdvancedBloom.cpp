#include "shade_pch.h"
#include "OpenGL_PAdvancedBloom.h"

shade::OpenGL_PAdvancedBloom::OpenGL_PAdvancedBloom()
{
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA16F,
		  shade::FrameBuffer::Texture::Format::RGBA16F,
		  shade::FrameBuffer::Texture::Format::RGBA16F,
		}));
}

void shade::OpenGL_PAdvancedBloom::Process()
{
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	std::uint32_t x = ceil((float)m_InputFrameBuffer->GetLayout().Width / 16.0f);
	std::uint32_t y = ceil((float)m_InputFrameBuffer->GetLayout().Height / 16.0f);

	//_CalculateGaussianCoef(m_Data);
	//m_GaussianUniformBuffer->SetData(&m_Data, sizeof(PPAdvancedBloom::ShaderData), 0);

	if (m_BloomFrameBuffer->GetLayout().Width  != m_InputFrameBuffer->GetLayout().Width ||
		m_BloomFrameBuffer->GetLayout().Height != m_InputFrameBuffer->GetLayout().Height)
	{
		m_BloomFrameBuffer->Resize(m_InputFrameBuffer->GetLayout().Width, m_InputFrameBuffer->GetLayout().Height);
	}

	
	m_BloomShader->Bind();
	glBindImageTexture(1, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE,  GL_RGBA16F);
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SelectSubrutine("s_Stage", "HDR", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	///////////////////////////////////////

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 0);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 2, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 1);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 2.f), ceil(y / 2.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 3, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 2);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 3.f), ceil(y / 3.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 4, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 3);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 4.f), ceil(y / 4.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 5, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 4);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 5.f), ceil(y / 5.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 6, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 5);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 6.f), ceil(y / 6.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 7, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 6);
	m_BloomShader->SelectSubrutine("s_Stage", "Down", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 7.f), ceil(y / 7.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	/// <summary>
	/// ///////////////
	/// </summary>
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 6, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 7);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 7.f), ceil(y / 7.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 5, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 6);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 6.f), ceil(y / 6.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 4, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 5);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 5.f), ceil(y / 5.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 3, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 4);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 4.f), ceil(y / 4.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(1), 2, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 3);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 3.f), ceil(y / 3.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 2);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(ceil(x / 2.f), (y / 2.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 1);
	m_BloomShader->SelectSubrutine("s_Stage", "Up", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Lod", 0);
	m_BloomShader->SelectSubrutine("s_Stage", "Blur", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	m_BloomShader->Bind();
	glBindImageTexture(1, m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

	m_BloomShader->SelectSubrutine("s_Stage", "Combine", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	/*glActiveTexture(GL_TEXTURE0 + 0); 
	glBindTexture(GL_TEXTURE_2D,  m_InputFrameBuffer->GetAttachment(0));
	
	glBindImageTexture(3,         m_BloomFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 0);// из 0 мипы в 1
	m_BloomShader->SelectSubrutine("s_Stage", "HDR", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	
	for (auto i = 1; i < 7; i++)
	{
		
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));

		glBindImageTexture(3,        m_BloomFrameBuffer->GetAttachment(1), i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		m_BloomShader->SendInt("u_Scale", i);// из 1 мипы в 2
		m_BloomShader->SelectSubrutine("s_Stage", "DBox4", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(x, y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}*/

	/*glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(1,        m_InputFrameBuffer->GetAttachment(1), 1, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA16F);
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(2), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);*/

	
	/*glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D,  m_BloomFrameBuffer->GetAttachment(1));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D,  m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,         m_BloomFrameBuffer->GetAttachment(2),  6, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 7); 
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(2));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,        m_BloomFrameBuffer->GetAttachment(0), 5, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 6);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(0));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,        m_BloomFrameBuffer->GetAttachment(2), 4, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 5);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(2));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,        m_InputFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 4);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(0));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,		 m_BloomFrameBuffer->GetAttachment(2), 2, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 3);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(2));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,        m_BloomFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 2);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(0));
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));
	glBindImageTexture(3,		 m_BloomFrameBuffer->GetAttachment(2), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 1);
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/

	/*for (auto i = 8; i > 1; i--)
	{
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_BloomFrameBuffer->GetAttachment(1));

		glBindImageTexture(3,        m_BloomFrameBuffer->GetAttachment(1), i - 2, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		m_BloomShader->SendInt("u_Scale", i - 1); // из 4 мипы в 3
		m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(x, y, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	}*/
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	/*glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D,	m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,			m_InputFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 1);// из 1 мипы в 2
	m_BloomShader->SelectSubrutine("s_Stage", "DBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(0), 3, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 2);// из 1 мипы в 2
	m_BloomShader->SelectSubrutine("s_Stage", "DBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(0), 4, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 3);// из 1 мипы в 2
	m_BloomShader->SelectSubrutine("s_Stage", "DBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);


	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(0),    3, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 4); // из 4 мипы в 3
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(0), 2, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 3); // из 3 мипы в 2
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,		 m_InputFrameBuffer->GetAttachment(0), 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 2); // из 3 мипы в 2
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);

	// Когда апскейл то ласт елемент 1 почему то, значит эа цифра означаает от куда куда скейлить  1 мипа в 0 а там из 0 в 1 даунскейлить
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(0));
	glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	m_BloomShader->SendInt("u_Scale", 1); // из 1 мипы в 0
	m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);*/

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	/*glBindImageTexture(2, m_BloomFrameBuffer->GetAttachment(1), 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(3, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	m_BloomShader->SelectSubrutine("s_Stage", "Combine", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/

	//glGenerateTextureMipmap(m_InputFrameBuffer->GetAttachment(1));
	

	/*for (auto i = 0; i < 6; i++)
	{
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		m_BloomShader->SendInt("u_Scale", i);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
		glBindImageTexture(2,        m_InputFrameBuffer->GetAttachment(1), i, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
		m_BloomShader->SelectSubrutine("s_Stage", "DBox4", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(x, y, 1);
	}*/

	/*glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);
	m_BloomShader->SendInt("u_Scale", 1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->SelectSubrutine("s_Stage", "UTent", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);*/

	/*for (auto i = 5; i > 0; i--)
	{
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		m_BloomShader->SendInt("u_Scale", i);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, m_InputFrameBuffer->GetAttachment(1));
		glBindImageTexture(2,		 m_BloomFrameBuffer->GetAttachment(1), i - 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
		m_BloomShader->SelectSubrutine("s_Stage", "UBox4", Shader::Type::Compute);
		m_BloomShader->ExecuteSubrutines();
		m_BloomShader->DispatchCompute(x, y, 1);
	}*/
	
	/*glMemoryBarrier(GL_ALL_BARRIER_BITS);

	m_BloomShader->SendInt("u_Scale", 0);
	glBindImageTexture(1, m_InputFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	m_BloomShader->SelectSubrutine("s_Stage", "Combine", Shader::Type::Compute);
	m_BloomShader->ExecuteSubrutines();
	m_BloomShader->DispatchCompute(x, y, 1);*/


	/* Поменять буфер формат атачмента, посмотреть как в старой версии стоит офсет на увеличение потому что минусы как то работуют странно*/
	/* Проверить верность тех координат когда идет комбайн, потому что выглядит как не верно*/
	/* / или * 2 похоже не верны */
	/* не хватате одного апскейла + пробрасыать через буфер се надо и не через один походу мип мапы не те стоят + умножение дилени ен забыть на текс коорд*/



	
	//glBindImageTexture(2, m_InputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);


	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
