#include "shade_pch.h"
#include "OpenGLRenderAPI.h"

#include <glad/glad.h>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         SHADE_CORE_WARNING(message);		return;
	case GL_DEBUG_SEVERITY_MEDIUM:       SHADE_CORE_WARNING(message);	return;
	case GL_DEBUG_SEVERITY_LOW:          SHADE_CORE_WARNING(message);	return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: SHADE_CORE_TRACE(message);		return;
	}
}

void shade::OpenGLRenderAPI::Init()
{
#if SHADE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);

	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
	glLineWidth(5.f);
	m_BloomFrameBuffer = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(1, 1,
		{ shade::FrameBuffer::Texture::Format::RGBA8,
		  shade::FrameBuffer::Texture::Format::RGBA8, 
		}));
}

void shade::OpenGLRenderAPI::ShutDown()
{
}

unsigned int shade::OpenGLRenderAPI::GetVideoMemoryUsage()
{
	// Nvidia only !
	GLint nTotalMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
		&nTotalMemoryInKB);

	GLint nCurAvailMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
		&nCurAvailMemoryInKB);

	return  (nTotalMemoryInKB - nCurAvailMemoryInKB) / 1024;
}

void shade::OpenGLRenderAPI::SetClearColor(const float& r, const float& g, const float& b, const float& a)
{
	glClearColor(r, g, b, a);
}

void shade::OpenGLRenderAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void shade::OpenGLRenderAPI::SetViewPort(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
{
	glViewport(x, y, width, height);
}

void shade::OpenGLRenderAPI::BeginScene(const Shared<Camera>& camera, const Shared<Environment>* env, const std::size_t& envCount)
{

}

void shade::OpenGLRenderAPI::EndScene()
{
	
}

void shade::OpenGLRenderAPI::DrawIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO) const
{

	VAO->Bind(); glDrawElements(static_cast<GLenum>(mode), IBO->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void shade::OpenGLRenderAPI::DrawInstanced(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const Shared<IndexBuffer>& IBO, const std::uint32_t& instanceCount)
{
	VAO->Bind(); glDrawElementsInstanced(static_cast<GLenum>(mode), IBO->GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
}

void shade::OpenGLRenderAPI::DrawNotIndexed(const Drawable::DrawMode& mode, const Shared<VertexArray>& VAO, const std::uint32_t& count)
{
	VAO->Bind();  glDrawArrays(static_cast<GLenum>(mode), 0, count);
}

void shade::OpenGLRenderAPI::Begin()
{
	Clear();
}

void shade::OpenGLRenderAPI::End()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shade::OpenGLRenderAPI::DepthTest(const bool& enable)
{
	(enable) ? glEnable(GL_DEPTH_TEST): glDisable(GL_DEPTH_TEST);
}

void shade::OpenGLRenderAPI::Bloom(const Shared<FrameBuffer>& inputFrameBuffer, const Shared<FrameBuffer>& outFrameBuffer, const Shared<Shader>& bloomShader)
{

	//Shared<FrameBuffer> Ping = shade::FrameBuffer::Create(shade::FrameBuffer::Layout(outFrameBuffer->GetLayout().Width, outFrameBuffer->GetLayout().Height, { shade::FrameBuffer::Texture::Format::RGBA8}));
	if (m_BloomFrameBuffer->GetLayout().Width != inputFrameBuffer->GetLayout().Width ||
		m_BloomFrameBuffer->GetLayout().Height != inputFrameBuffer->GetLayout().Height)
	{
		m_BloomFrameBuffer->Resize(inputFrameBuffer->GetLayout().Width, inputFrameBuffer->GetLayout().Height);
	}
			

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	std::uint32_t w = ceil((float)inputFrameBuffer->GetLayout().Width / 16.0f);
	std::uint32_t h = ceil((float)inputFrameBuffer->GetLayout().Height / 16.0f);

	bloomShader->Bind();
	bloomShader->SelectSubrutine("u_Blur",  "Horizontal", shade::Shader::Type::Compute);
	bloomShader->SelectSubrutine("u_Stage", "GetHDR", shade::Shader::Type::Compute);
	bloomShader->ExecuteSubrutines();
	//PingPong->Bind();
	glBindImageTexture(0, inputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	bloomShader->DispatchCompute(w, h, 1);

	bloomShader->SelectSubrutine("u_Blur",  "Horizontal", shade::Shader::Type::Compute);
	bloomShader->SelectSubrutine("u_Stage", "ProcessBlur", shade::Shader::Type::Compute);
	bloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY,  GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	bloomShader->DispatchCompute(w, h, 1);

	bloomShader->SelectSubrutine("u_Blur",  "Vertical",    shade::Shader::Type::Compute);
	bloomShader->SelectSubrutine("u_Stage", "ProcessBlur", shade::Shader::Type::Compute);
	bloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	bloomShader->DispatchCompute(w, h, 1);

	// Combine stage
	bloomShader->SelectSubrutine("u_Blur",  "Vertical", shade::Shader::Type::Compute);
	bloomShader->SelectSubrutine("u_Stage", "Combine", shade::Shader::Type::Compute);
	bloomShader->ExecuteSubrutines();
	glBindImageTexture(0, m_BloomFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, inputFrameBuffer->GetAttachment(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	bloomShader->DispatchCompute(w, h, 1);
}
